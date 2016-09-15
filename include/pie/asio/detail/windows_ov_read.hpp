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
                auto io_data_ptr = pie::asio::io_operation_data::create(io_operation::IO_ERROR);
                if (io_data_ptr == nullptr)
                {
                    ec = std::error_code(ERROR_NOT_ENOUGH_MEMORY, std::system_category());
                }
                else
                {
                    io_data_ptr->buffer.resize(size);
                    io_data_ptr->on_read = std::move(on_read);

                    WSABUF buf;
                    buf.buf = const_cast<char *>(io_data_ptr->buffer.c_str());
                    buf.len = io_data_ptr->buffer.size();
                    DWORD bytes_received = 0;
                    DWORD flags = 0;

                    int result = ::WSARecv(socket.get_handle(), &buf, 1, &bytes_received, &flags, reinterpret_cast<WSAOVERLAPPED*>(&io_data_ptr->ov), nullptr);
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
