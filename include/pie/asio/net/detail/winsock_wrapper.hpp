#pragma once

#include <system_error>

#include <pie/asio/net/address_family.hpp>
#include <pie/asio/net/socket_protocol.hpp>
#include <pie/asio/net/socket_type.hpp>

#include <pie/asio/config.hpp>

#include <MSWSock.h>

namespace pie
{
    namespace asio
    {
        namespace net
        {
            namespace detail
            {
                inline native_socket_type socket(pie::asio::net::address_family family,
                                                 pie::asio::net::socket_type type,
                                                 pie::asio::net::socket_protocol protocol,
                                                 std::error_code & ec) noexcept
                {
                    SOCKET s = ::WSASocketW(family, type, protocol, nullptr, 0, WSA_FLAG_OVERLAPPED);
                    if (s == INVALID_SOCKET)
                        ec = std::error_code(::WSAGetLastError(), std::system_category());

                    return s;
                }

                inline void shutdown(native_socket_type s, int how, std::error_code & ec) noexcept
                {
                    if (::shutdown(s, how) == SOCKET_ERROR)
                    {
                        ec = std::error_code(::WSAGetLastError(), std::system_category());
                        switch (ec.value())
                        {
                        case WSAEINPROGRESS:
                            ec = std::make_error_code(std::errc::operation_in_progress);
                            break;

                        case WSAECONNABORTED:
                            ec = std::make_error_code(std::errc::connection_aborted);
                            break;

                        case WSAECONNRESET:
                            ec = std::make_error_code(std::errc::connection_reset);
                            break;

                        case WSAENETDOWN:
                            ec = std::make_error_code(std::errc::network_down);
                        default:
                            break;
                        }                        
                    }
                    else
                    {
                        ec = std::error_code();
                    }
                }

                inline void closesocket(SOCKET handle) noexcept
                {
                    if (::closesocket(handle) == SOCKET_ERROR)
                    {
                        std::error_code error = std::error_code(::WSAGetLastError(), std::system_category());
                        
                        int x = 0;
                    }
                }

                inline bool set_nonblocking(SOCKET handle, std::error_code & ec) noexcept
                {
                    unsigned long arg = -1;
                    if (::ioctlsocket(handle, FIONBIO, &arg) == SOCKET_ERROR)
                    {
                        ec = std::error_code(::WSAGetLastError(), std::system_category());
                        return false;
                    }

                    return true;
                }

                inline bool update_connect_context(SOCKET handle) noexcept
                {
                    return ::setsockopt(handle, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0) != SOCKET_ERROR;
                }
            }
        }
    }
}
