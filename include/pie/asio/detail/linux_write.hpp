#pragma once

#include <memory>
#include <system_error>

#include <pie/asio/net/socket.hpp>
#include <pie/asio/io_operation.hpp>

#include <unistd.h>

namespace pie
{
    namespace asio
    {
        namespace detail
        {
            /*
             * Writes the specified buffer to the socket.
             * The buffer is not copied - therefore, the user is responsible for managing its lifetime.
             *
             * @param socket The socket object.
             * @param buffer A pointer to the buffer,
             * @param size The size of the buffer.
             * @param on_write A function object that will be called on completion or i/o error.
             * @param ec An object that will hold any errors.
             *
             * @return True on success; otherwise false.
             */
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

                    io_data_ptr->fd = socket.get_handle();
                    // Directly use the raw buffer
                    ssize_t result = get_errno_error(::write(io_data_ptr->fd, buffer, size), ec);
                    if (result > 0)
                    {
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }

                    if (ec.value() == EAGAIN || ec.value() == EWOULDBLOCK)
                    {
                        ec = make_error_code(std::errc::operation_in_progress);
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }
                }

                return false;
            }

            /*
             * Writes the specified buffer to the socket.
             * The buffer is copied and managed by the library.
             *
             * @param socket The socket object.
             * @param buffer A string containiing a buffer.
             * @param on_write A function object that will be called on completion or i/o error.
             * @param ec An object that will hold any errors.
             *
             * @return True on success; otherwise false.
             */
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
                    io_data_ptr->fd = socket.get_handle();
                    io_data_ptr->operation = io_operation::IO_WRITE;
                    io_data_ptr->buffer = buffer;
                    io_data_ptr->on_write = std::move(on_write);

                    ssize_t result = get_errno_error(::write(io_data_ptr->fd, io_data_ptr->buffer.c_str(), io_data_ptr->buffer.size()));
                    if (result > 0)
                    {
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }

                    if (ec.value() == EAGAIN || ec.value() == EWOULDBLOCK)
                    {
                        ec = make_error_code(std::errc::operation_in_progress);
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }
                }

                return false;
            }

            /*
             * Writes the specified buffer to the socket.
             * The buffer is moved and managed by the library.
             *
             * @param socket The socket object.
             * @param buffer A string containiing a buffer.
             * @param on_write A function object that will be called on completion or i/o error.
             * @param ec An object that will hold any errors.
             *
             * @return True on success; otherwise false.
             */
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
                    
                    ssize_t result = get_errno_error(::write(io_data_ptr->fd, io_data_ptr->buffer.c_str(), io_data_ptr->buffer.size()));
                    if (result > 0)
                    {
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }

                    if (ec.value() == EAGAIN || ec.value() == WSAEWOULDBLOCK)
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
