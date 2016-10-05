#pragma once

#include <sys/types.h>
#include <sys/socket.h>

#include <pie/asio/get_error.hpp>
#include <pie/asio/io_service.hpp>
#include <pie/asio/context_manager.hpp>

namespace pie
{
    namespace asio
    {
        namespace detail
        {
            inline bool write(pie::asio::net::socket const & socket,
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

                    ssize_t result = get_errno_error(::send(socket.get_handle(), buffer, size, 0), ec);
                    if (result != -1)
                    {
                        ec = std::error_code();
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }

                    if (ec.value() == EAGAIN || ec.value() == EWOULDBLOCK)
                    {
                        ec = make_error_code(std::errc::operation_in_progress);
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }

                    context_manager.assign_free_context(std::move(io_data_ptr));
                }

                return false;
            }

            inline bool write(pie::asio::net::socket const & socket,
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
                    io_data_ptr->on_write = std::move(on_write);
                    io_data_ptr->buffer = buffer;

                    ssize_t result = get_errno_error(::send(socket.get_handle(), buffer.c_str(), buffer.size(), 0), ec);
                    if (result != -1)
                    {
                        ec = std::error_code();
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }

                    if (ec.value() == EAGAIN || ec.value() == EWOULDBLOCK)
                    {
                        ec = make_error_code(std::errc::operation_in_progress);
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }

                    context_manager.assign_free_context(std::move(io_data_ptr));
                }

                return false;
            }

            inline bool write(pie::asio::net::socket const & socket,
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
                    io_data_ptr->on_write = std::move(on_write);
                    io_data_ptr->buffer = std::move(buffer);

                    ssize_t result = get_errno_error(::send(socket.get_handle(), buffer.c_str(), buffer.size(), 0), ec);
                    if (result != -1)
                    {
                        ec = std::error_code();
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }

                    if (ec.value() == EAGAIN || ec.value() == EWOULDBLOCK)
                    {
                        ec = make_error_code(std::errc::operation_in_progress);
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }

                    context_manager.assign_free_context(std::move(io_data_ptr));
                }

                return false;
            }
        }
    }
}

