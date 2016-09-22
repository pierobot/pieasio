#pragma once

#include <memory>
#include <system_error>

#include <pie/asio/net/socket.hpp>
#include <pie/asio/io_operation.hpp>

namespace pie
{
    namespace asio
    {
        namespace detail
        {
            bool read(pie::asio::net::socket const & socket,
                      std::size_t size,
                      pie::asio::on_read_type && on_read,
                      std::error_code & ec)
            {
                auto & io_service = pie::asio::net::get_io_service(socket);
                auto & context_manager = pie::asio::get_context_manager(io_service);

                auto io_data_ptr = context_manager.get_free_context();
                if (io_data_ptr == nullptr)
                {
                    ec = std::error_code(ERROR_NOT_ENOUGH_MEMORY, std::system_category());
                }
                else
                {
                    io_data_ptr->operation = io_operation::IO_READ;
                    io_data_ptr->buffer.resize(size);
                    io_data_ptr->on_read = std::move(on_read);

                    io_data_ptr->wsabuf.buf = const_cast<char *>(io_data_ptr->buffer.c_str());
                    io_data_ptr->wsabuf.len = io_data_ptr->buffer.size();
                    DWORD bytes_received = 0;
                    DWORD flags = 0;

                    int result = ::WSARecv(socket.get_handle(), &io_data_ptr->wsabuf, 1, &bytes_received, &flags, reinterpret_cast<WSAOVERLAPPED*>(&io_data_ptr->ov), nullptr);
                    if (result != SOCKET_ERROR)
                    {
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }

                    ec = std::error_code(::WSAGetLastError(), std::system_category());
                    if (ec.value() == WSA_IO_PENDING)
                    {
                        ec = std::make_error_code(std::errc::operation_in_progress);
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }
                }

                return false;
            }
        }
    }
}
