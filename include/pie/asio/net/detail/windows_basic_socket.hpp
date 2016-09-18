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
        typedef SOCKET native_handle;
        
        basic_socket() noexcept :
            m_handle(INVALID_SOCKET)
        {
        }
        
        basic_socket(pie::asio::net::address_family family, std::error_code & ec = std::error_code()) noexcept :
            m_family(family),
            m_handle(pie::asio::net::detail::create_socket(family, pie::asio::net::socket_type::stream, pie::asio::net::socket_protocol::tcp, ec))
        {
        }

        virtual ~basic_socket()
        {
            if (m_handle != INVALID_SOCKET)
                pie::asio::net::detail::close_socket(m_handle);
        }
        
        virtual native_handle get_handle() const noexcept override final
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
        native_handle m_handle;
        address_family m_family;
    };
}
}
}
}
