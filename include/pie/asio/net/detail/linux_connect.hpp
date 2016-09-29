#pragma once

// --------------------------------------
//
// FIX SOCKET BINDER TO USE getaddrinfo
//
// --------------------------------------

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

                        sockaddr_in addr_in{};
                        addr_in.sin_port = ::htons(resolver.get_port());
                        addr_in.sin_family = socket.get_family();
                        addr_in.sin_addr.S_un.S_addr = INADDR_ANY;

                        int result = get_errno_error(::bind(socket.get_handle(), reinterpret_cast<sockaddr*>(&addr_in), sizeof(sockaddr)), ec);
                        if (result == ~0)
                        {
                            return false;
                        }

                        addr_in.sin_addr.S_un.S_addr = resolver.get_addr();

                        // Attempt to connect
                        result = get_errno_error(::connect(socket.get_handle(), reinterpret_cast<sockaddr*>(&addr_in), sizeof(sockaddr)), ec);
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

