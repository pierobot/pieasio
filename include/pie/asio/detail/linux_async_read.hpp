#pragma once

#include <pie/asio/get_error.hpp>
#include <pie/asio/io_service.hpp>
#include <pie/asio/context_manager.hpp>

#include <unistd.h>

namespace pie
{
    namespace asio
    {
        namespace detail
        {
            inline bool read(pie::asio::net::socket const & socket,
                             std::size_t size,
                             pie::asio::on_read_type && on_read,
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
                    io_data_ptr->operation = io_operation::IO_READ;
                    io_data_ptr->buffer.resize(size);
                    io_data_ptr->on_read = std::move(on_read);

                    io_data_ptr->fd = socket.get_handle();

                    // Attempt to receive any data
                    ssize_t result = get_errno_error(::recv(socket.get_handle(), &io_data_ptr->buffer[0], size, 0), ec);
                    // Was the operation successful?
                    if (result == 0)
                    {
                        // Yes
                        ec = std::error_code();
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }

                    // No, but the operation might be pending
                    if (ec.value() == EAGAIN || ec.value() == EWOULDBLOCK)
                    {
                        ec = std::make_error_code(std::errc::operation_in_progress);
                        context_manager.assign_pending_context(std::move(io_data_ptr));
                        return true;
                    }

                    // Operation failed
                    context_manager.assign_free_context(std::move(io_data_ptr));
                }

                return false;
            }
        }
    }
}

