#pragma once

#include <pie/asio/config.hpp>
#include <pie/asio/get_error.hpp>
#include <pie/asio/net/detail/socket.hpp>
#include <pie/asio/associable.hpp>

#include <system_error>
#include <string>

namespace pie {
namespace asio {
namespace net {
namespace detail
{
    class basic_socket : public pie::asio::associable_object<native_socket_type>
    {
    public:
        /*
         * Default constructor. The object is in an invalid state.
         */
        basic_socket() noexcept :
            m_handle(INVALID_SOCKET),
            m_owns(false)
        {
        }

        /*
         * Construct from a native socket descriptor.
         * Does not assume ownership of the socket descriptor.
         * Caller is responsible for closing it.
         *
         * @param s The socket descriptor.
         */
        basic_socket(native_handle_type s) noexcept :
            m_handle(s),
            m_owns(false)
        {
        }

        /*
         * The constructor that will generally be used.
         * The internal socket descriptor is owned by this object.
         * If given an existing address, the socket will reuse it by setting the SO_REUSEADDR option.
         *
         * @param family The address family.
         * @param ec The object that will hold errors.
         */
        basic_socket(pie::asio::net::address_family family, std::error_code & ec) noexcept :
            m_handle(pie::asio::net::detail::socket(family, pie::asio::net::socket_type::stream, pie::asio::net::socket_protocol::tcp, ec)),
            m_family(family),
            m_owns(true)
        {
            if (m_handle != INVALID_SOCKET)
            {
                int enable = 1;
#if defined(_WIN32)
                get_wsa_error(::setsockopt(m_handle, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&enable), sizeof(enable)), ec);
#elif defined(__linux__)
                get_errno_error(::setsockopt(m_handle, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)), ec);
#endif
            }
        }

        virtual ~basic_socket()
        {
            // Initiate a graceful shutdown if this object owns the socket
            if (m_owns == true && m_handle != INVALID_SOCKET)
            {
                std::error_code error;
#if defined(_WIN32)
                pie::asio::net::detail::shutdown(m_handle, SD_SEND, error);
#elif defined(__linux___)
                pie::asio::net::detail::shutdown(m_handle, SHUT_WR, error);
#endif
            }
        }

        /*
         * Gets the value of the internal socket descriptor.
         *
         * @return the native socket descriptor.
         */
        virtual native_handle_type get_handle() const noexcept override final
        {
            return m_handle;
        }

        /*
         * Gets the enum value associated with the address family.
         *
         * @return the value of the address family.
         */
        pie::asio::net::address_family get_family() const noexcept
        {
            return m_family;
        }

        /*
         * Sets the socket to be nonblocking.
         * Used in UNIX-like operating systems and in Windows, where overlapped operations are not supported.
         *
         * @param ec The object that will hold any error.
         */
        bool set_nonblocking(std::error_code & ec) noexcept
        {
            if (detail::set_nonblocking(m_handle, ec) == false)
            {
                m_handle = INVALID_SOCKET;
                return false;
            }

            return true;
        }
    protected:
    private:
        native_handle_type m_handle;
        address_family m_family;
        bool m_owns;
    };
}
}
}
}
