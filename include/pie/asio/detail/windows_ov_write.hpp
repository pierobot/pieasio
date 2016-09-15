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
                auto io_data_ptr = pie::asio::io_operation_data::create(io_operation::IO_WRITE);
                if (io_data_ptr == nullptr)
                {
                    ec = std::error_code(ERROR_NOT_ENOUGH_MEMORY, std::system_category());
                }
                else
                {
                    WSABUF buf;
                    buf.buf = const_cast<char *>(buffer);
                    buf.len = size;

                    io_data_ptr->on_write = std::move(on_write);
                    
                    int result = ::WSASend(socket.get_handle(), &buf, 1, nullptr, 0, reinterpret_cast<WSAOVERLAPPED*>(&io_data_ptr->ov), nullptr);
                    if (result != SOCKET_ERROR)
                    {
                        io_data_ptr.release();
                        return true;
                    }

                    ec = std::error_code(::WSAGetLastError(), std::system_category());
                    if (ec.value() == WSA_IO_PENDING)
                    {
                        io_data_ptr.release();
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
                auto io_data_ptr = pie::asio::io_operation_data::create(io_operation::IO_WRITE);
                if (io_data_ptr == nullptr)
                {
                    ec = std::error_code(ERROR_NOT_ENOUGH_MEMORY, std::system_category());
                }
                else
                {
                    io_data_ptr->buffer = buffer;
                    io_data_ptr->on_write = std::move(on_write);

                    WSABUF buf;
                    buf.buf = const_cast<char *>(io_data_ptr->buffer.c_str());
                    buf.len = io_data_ptr->buffer.size();

                    int result = ::WSASend(socket.get_handle(), &buf, 1, nullptr, 0, reinterpret_cast<WSAOVERLAPPED*>(&io_data_ptr->ov), nullptr);
                    if (result != SOCKET_ERROR)
                    {
                        io_data_ptr.release();
                        return true;
                    }

                    ec = std::error_code(::WSAGetLastError(), std::system_category());
                    if (ec.value() == WSA_IO_PENDING)
                    {
                        io_data_ptr.release();
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
                auto io_data_ptr = pie::asio::io_operation_data::create(io_operation::IO_WRITE);
                if (io_data_ptr == nullptr)
                {
                    ec = std::error_code(ERROR_NOT_ENOUGH_MEMORY, std::system_category());
                }
                else
                {
                    io_data_ptr->buffer = std::move(buffer);
                    io_data_ptr->on_write = std::move(on_write);

                    WSABUF buf;
                    buf.buf = const_cast<char *>(io_data_ptr->buffer.c_str());
                    buf.len = io_data_ptr->buffer.size();

                    int result = ::WSASend(socket.get_handle(), &buf, 1, nullptr, 0, reinterpret_cast<WSAOVERLAPPED*>(&io_data_ptr->ov), nullptr);
                    if (result != SOCKET_ERROR)
                    {
                        io_data_ptr.release();
                        return true;
                    }

                    ec = std::error_code(::WSAGetLastError(), std::system_category());
                    if (ec.value() == WSA_IO_PENDING)
                    {
                        io_data_ptr.release();
                        return true;
                    }
                }

                return false;
            }
        }
    }
}
