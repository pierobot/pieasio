#pragma once

#include <pie/asio/net/detail/winsock_wrapper.hpp>
#include <pie/asio/associable.hpp>

#include <system_error>
#include <string>

namespace pie {
namespace asio {
namespace net {
namespace detail
{
    class basic_socket : public pie::asio::associable_object<SOCKET>
    {
    public:
        basic_socket() noexcept :
            m_handle(INVALID_SOCKET),
            m_owns(false)
        {
        }

        basic_socket(native_handle_type s) noexcept :
            m_handle(s),
            m_owns(false)
        {
        }
        
        basic_socket(pie::asio::net::address_family family, std::error_code & ec = std::error_code()) noexcept :
            m_family(family),
            m_handle(pie::asio::net::detail::socket(family, pie::asio::net::socket_type::stream, pie::asio::net::socket_protocol::tcp, ec)),
            m_owns(true)
        {
            if (m_handle != INVALID_SOCKET)
            {
                int enable = 1;
                if (::setsockopt(m_handle, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&enable), sizeof(enable)) == SOCKET_ERROR)
                    ec = std::error_code(::WSAGetLastError(), std::system_category());
            }
        }

        virtual ~basic_socket()
        {
            // Initiate a graceful shutdown if this object owns the socket
            if (m_owns == true && m_handle != INVALID_SOCKET)
            {
                std::error_code error;
                pie::asio::net::detail::shutdown(m_handle, SD_SEND, error);
            }
        }
        
        virtual native_handle_type get_handle() const noexcept override final
        {
            return m_handle;
        }

        pie::asio::net::address_family get_family() const noexcept
        {
            return m_family;
        }

        bool set_nonblocking(std::error_code & ec = std::error_code()) noexcept
        {
            if (detail::set_nonblocking(m_handle, ec) == false)
            {
                m_handle = INVALID_SOCKET;
                ec = std::error_code(::WSAGetLastError(), std::system_category());
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
