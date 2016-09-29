#pragma once

#include <pie/asio/net/address_family.hpp>
#include <pie/asio/get_error.hpp>
#include <pie/asio/net/socket_protocol.hpp>
#include <pie/asio/net/socket_type.hpp>

#include <pie/asio/config.hpp>
#include <pie/asio/translate_error.hpp>

#if defined(_WIN32)
#   include <MSWSock.h>
#elif defined(__linux__)
#   include <errno.h>
#   include <unistd.h>
#   include <sys/socket.h>
#   include <stropts.h>

#   define INVALID_SOCKET (~0)
#   define SOCKET_ERROR   (-1)
#endif

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
#if defined(_WIN32)
                    native_socket_type s = get_wsa_error(::WSASocketW(family, type, protocol, nullptr, 0, WSA_FLAG_OVERLAPPED), ec);
#elif defined(__linux__)
                    native_socket_type s = get_errno_error(::socket(family, type, protocol), ec);
#endif
                    return s;
                }

                inline int shutdown(native_socket_type s, int how, std::error_code & ec) noexcept
                {
#if defined(_WIN32)
                    int result = get_wsa_error(::shutdown(s, how), ec);
#elif defined(__linux__)
                    int result = get_errno_error(::shutdown(s, how), ec);
#endif                  
                    return result;
                }

                inline int close(native_socket_type s, std::error_code & ec) noexcept
                {
#if defined(_WIN32)
                    int result = get_wsa_error(::closesocket(s), ec);
#elif defined(__linux__)
                    int result = get_errno_error(::close(s), ec);
#endif

                    return result;
                }

                inline int set_nonblocking(native_socket_type s, std::error_code & ec) noexcept
                {
#if defined(_WIN32)
                    unsigned long arg = 1;
                    int result = get_wsa_error(::ioctlsocket(s, FIONBIO, &arg), ec);
#elif defined(__linux__)
                    int result = get_errno_error(::ioctl(s, FIONBIO, &arg), ec);
#endif

                    return result;
                }

#if defined(_WIN32)
                inline int update_connect_context(native_socket_type s, std::error_code & ec) noexcept
                {
                    return get_wsa_error(::setsockopt(s, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0), ec);
                }
#endif
            }
        }
    }
}
