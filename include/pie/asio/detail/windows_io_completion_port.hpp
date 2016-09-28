#pragma once

#include <iostream>
#include <memory>
#include <mutex>
#include <unordered_map>

#include <pie/asio/detail/basic_poller.hpp>
#include <pie/asio/config.hpp>
#include <pie/asio/net/detail/socket.hpp>

#include <Windows.h>

namespace pie {
namespace asio {
namespace detail {
    
    class io_completion_port : public basic_poller
    {
    public:
        io_completion_port() noexcept :
            basic_poller(),
            m_handle(::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0))
        {
        }

        virtual ~io_completion_port()
        {
            ::CloseHandle(m_handle);
        }

        bool associate(native_handle_type object_handle, std::error_code & ec)
        {
            // Associate the object and custom context with the completion port
            if (::CreateIoCompletionPort(object_handle, m_handle, reinterpret_cast<ULONG_PTR>(object_handle), 0) == m_handle)
            {
                // Successfully associated the object with the completion port
                ec = std::error_code();
                return true;
            }
            else
            {
                // Failed to associate the object with the completion port
                ec = std::error_code(::WSAGetLastError(), std::system_category());
            }

            return false;
        }

        virtual std::size_t poll() override final
        {
            DWORD bytes_transferred = 0;
            ULONG_PTR key = 0;
            LPOVERLAPPED overlapped = nullptr;
            
            // Attempt to dequeue a completion packet
            BOOL success = ::GetQueuedCompletionStatus(m_handle, &bytes_transferred, &key, &overlapped, 500);
            // Was there an error?
            if (success == TRUE)
            {
                // No
                // Was the completion port requested to be gracefully closed by us?
                native_handle_type handle = reinterpret_cast<native_handle_type>(key);
                if (handle == m_handle)
                {
                    // Yes, but wait for any outstanding operations to complete
                    return 0;
                }

                // Enqueue the operation
                on_event(handle, overlapped, bytes_transferred);
                // Dequeue a pending operation
                auto fn = get_operation_manager().pop();
                fn();
            }
            else
            {
                DWORD flags = 0;
                // Is the socket shutting down?
                if (overlapped != nullptr && bytes_transferred == 0)
                {
                    // Yes
                    std::error_code close_error;
                    pie::asio::net::detail::close(static_cast<native_socket_type>(key), close_error);
                }

                // No, do further checking
                int last_error = ::WSAGetLastError();
                // Did the wait time-out?
                if (last_error != ERROR_ABANDONED_WAIT_0)
                {
                    // No - unhandled error
                    return 0;
                }

                // Did we utterly fail to dequeue a completion packet?
                if (overlapped == nullptr)
                {
                    // Yes
                    std::cout << "COMPLETION PACKET FAILURE";
                }
                else
                {
                    // No. 
                }
            }

            return bytes_transferred;
        }

        void post() const
        {
        }
    protected:
    private:
        native_handle_type m_handle;

        void on_event(HANDLE handle, LPOVERLAPPED overlapped, DWORD bytes_transferred)
        {
            if (overlapped != nullptr)
            {
                context_pointer_type io_data_ptr = get_context_manager().get_pending_context(CONTAINING_RECORD(overlapped, pie::asio::io_operation_data, ov));
                if (io_data_ptr == nullptr)
                    return;

                switch (io_data_ptr->operation)
                {
                case io_operation::IO_CONNECT:
                {
                    get_operation_manager().push([=]() mutable
                    {
                        // Required. ConnectEx does not automatically update the socket context.
                        // We must do this to use the socket with functions such as getpeername, getsockname, getsockopt, setsockopt, and shutdown
                        std::error_code update_ec;
                        pie::asio::net::detail::update_connect_context(reinterpret_cast<native_socket_type>(handle), update_ec);
                        // Ensure the operation succeeded
                        if (!update_ec)
                        {
                            if (io_data_ptr->on_connect != nullptr)
                            {
                                // Call the connect handler
                                io_data_ptr->on_connect(std::error_code());
                            }
                        }

                        get_context_manager().assign_free_context(std::move(io_data_ptr));
                    });
                }
                break;

                case io_operation::IO_WRITE:
                {
                    get_operation_manager().push([=]() mutable
                    {
                        if (io_data_ptr->on_write != nullptr)
                        {
                            io_data_ptr->on_write(static_cast<std::size_t>(bytes_transferred), std::error_code());
                        }

                        io_data_ptr->buffer = std::string();
                        get_context_manager().assign_free_context(std::move(io_data_ptr));
                    });
                }
                break;

                case io_operation::IO_READ:
                {
                    get_operation_manager().push([=]() mutable
                    {
                        if (io_data_ptr->on_read != nullptr)
                        {
                            if (bytes_transferred > 0)
                            {
                                io_data_ptr->on_read(std::move(io_data_ptr->buffer), bytes_transferred, std::error_code());
                            }
                        }

                        get_context_manager().assign_free_context(std::move(io_data_ptr));
                    });
                }
                break;

                default:
                    break;
                }
            }
        }

        void on_error(LPOVERLAPPED overlapped)
        {

        }
    };
}
}
}
