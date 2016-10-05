#pragma once

#include <pie/asio/net/socket.hpp>
#include <pie/asio/io_operation.hpp>
#include <pie/asio/net/resolver.hpp>
#include <pie/asio/get_error.hpp>

namespace pie
{
    namespace asio
    {
        namespace net
        {
            namespace detail
            {
                bool connect(pie::asio::net::socket const & socket,
                             pie::asio::net::resolver const & resolver,
                             pie::asio::io_operation_data::on_connect_type && on_connect,
                             std::error_code & ec)
                {
                    if (resolver.valid() == true)
                    {
                        auto & io_service = pie::asio::net::get_io_service(socket);
                        auto & context_manager = pie::asio::get_context_manager(io_service);

                        auto io_data_ptr = context_manager.get_free_context();
                        if (io_data_ptr == nullptr)
                        {
                            ec = std::make_error_code(std::errc::not_enough_memory);
                            return false;
                        }

                        io_data_ptr->operation = io_operation::IO_CONNECT;
                        // Assign the on connect handler
                        io_data_ptr->on_connect = std::move(on_connect);

                        // Iterate through the possible addresses
                        for (auto address_info : resolver)
                        {
                            // Attempt to bind the socket to the current address
                            int result = get_errno_error(::bind(socket.get_handle(), address_info->ai_addr, sizeof(sockaddr)), ec);
                            if (result == ~0)
                            {
                                // Unsuccessful, try the next address
                                continue;
                            }

                            // Attempt to connect
                            result = get_errno_error(::connect(socket.get_handle(), address_info->ai_addr, sizeof(sockaddr)), ec);
                            // Was the operation successful?
                            if (result == 0)
                            {
                                // Yes
                                ec = std::error_code();
                                return true;
                            }
                            // No, but it could already be in progress
                            if (ec.value() == EINPROGRESS)
                            {
                                ec = std::make_error_code(std::errc::operation_in_progress);
                                return true;
                            }
                        }

                        // If execution reaches here, we were unable to bind to an address
                    }
                    else
                    {
                        ec = std::make_error_code(std::errc::not_enough_memory);
                    }

                    return false;
                }
            }
        }
    }
}

