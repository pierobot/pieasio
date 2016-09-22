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
            bool write(pie::asio::net::socket const & socket,
                       char const * buffer,
                       std::size_t size,
                       pie::asio::on_write_type && on_write,
                       std::error_code & ec)
            {
                auto & io_service = pie::asio::net::get_io_service(socket);
                auto & context_manager = pie::asio::get_context_manager(io_service);

                auto io_data_ptr = context_manager.get_free_context();
                if (io_data_ptr == nullptr)
                {
                    ec = std::make_error_code(std::errc::not_enough_memory);
                }
                else
                {
                    io_data_ptr->operation = io_operation::IO_WRITE;
                    io_data_ptr->on_write = std::move(on_write);

                    io_data_ptr->wsabuf.buf = const_cast<char *>(buffer);
                    io_data_ptr->wsabuf.len = size;
                    
                    int result = ::WSASend(socket.get_handle(), &io_data_ptr->wsabuf, 1, nullptr, 0, reinterpret_cast<WSAOVERLAPPED*>(&io_data_ptr->ov), nullptr);
                    if (result != SOCKET_ERROR)
                    {
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }

                    ec = std::error_code(::WSAGetLastError(), std::system_category());
                    if (ec.value() == WSA_IO_PENDING)
                    {
                        ec = make_error_code(std::errc::operation_in_progress);
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }
                }

                return false;
            }

            bool write(pie::asio::net::socket const & socket,
                       std::string const & buffer,
                       pie::asio::on_write_type && on_write,
                       std::error_code & ec)
            {
                auto & io_service = pie::asio::net::get_io_service(socket);
                auto & context_manager = pie::asio::get_context_manager(io_service);

                auto io_data_ptr = context_manager.get_free_context();
                if (io_data_ptr == nullptr)
                {
                    ec = std::make_error_code(std::errc::not_enough_memory);
                }
                else
                {
                    io_data_ptr->operation = io_operation::IO_WRITE;
                    io_data_ptr->buffer = buffer;
                    io_data_ptr->on_write = std::move(on_write);

                    io_data_ptr->wsabuf.buf = const_cast<char *>(io_data_ptr->buffer.c_str());
                    io_data_ptr->wsabuf.len = io_data_ptr->buffer.size();

                    int result = ::WSASend(socket.get_handle(), &io_data_ptr->wsabuf, 1, nullptr, 0, reinterpret_cast<WSAOVERLAPPED*>(&io_data_ptr->ov), nullptr);
                    if (result != SOCKET_ERROR)
                    {
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }

                    ec = std::error_code(::WSAGetLastError(), std::system_category());
                    if (ec.value() == WSA_IO_PENDING)
                    {
                        ec = make_error_code(std::errc::operation_in_progress);
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }
                }

                return false;
            }

            bool write(pie::asio::net::socket const & socket, 
                       std::string && buffer,
                       pie::asio::on_write_type && on_write,
                       std::error_code & ec)
            {
                auto & io_service = pie::asio::net::get_io_service(socket);
                auto & context_manager = pie::asio::get_context_manager(io_service);

                auto io_data_ptr = context_manager.get_free_context();
                if (io_data_ptr == nullptr)
                {
                    ec = std::make_error_code(std::errc::not_enough_memory);
                }
                else
                {
                    io_data_ptr->operation = io_operation::IO_WRITE;
                    io_data_ptr->buffer = std::move(buffer);
                    io_data_ptr->on_write = std::move(on_write);

                    io_data_ptr->wsabuf.buf = const_cast<char *>(io_data_ptr->buffer.c_str());
                    io_data_ptr->wsabuf.len = io_data_ptr->buffer.size();

                    int result = ::WSASend(socket.get_handle(), &io_data_ptr->wsabuf, 1, nullptr, 0, reinterpret_cast<WSAOVERLAPPED*>(&io_data_ptr->ov), nullptr);
                    if (result != SOCKET_ERROR)
                    {
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }

                    ec = std::error_code(::WSAGetLastError(), std::system_category());
                    if (ec.value() == WSA_IO_PENDING)
                    {
                        ec = make_error_code(std::errc::operation_in_progress);
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }
                }

                return false;
            }
        }
    }
}
