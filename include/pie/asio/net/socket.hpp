#pragma once

#include <pie/asio/io_service.hpp>
#include <pie/asio/net/detail/basic_socket.hpp>

namespace pie {
namespace asio {
namespace net {

    // Forward declarations
    class socket;
    pie::asio::io_service & get_io_service(pie::asio::net::socket const & s);

    class socket : public detail::basic_socket
    {
    public:
        socket(pie::asio::io_service & io_service) noexcept :
            detail::basic_socket(),
            m_io_service(io_service)
        {
        }

        socket(pie::asio::io_service & io_service, native_socket_type native_socket) noexcept :
            detail::basic_socket(native_socket),
            m_io_service(io_service)
        {
        }

        socket(pie::asio::io_service & io_service, pie::asio::net::address_family family, std::error_code & ec) noexcept :
            detail::basic_socket(family, ec),
            m_io_service(io_service)
        {
        }

        virtual ~socket()
        {
        }
    protected:
    private:
        pie::asio::io_service & m_io_service;       
        friend pie::asio::io_service & get_io_service(pie::asio::net::socket const & s);
    };

    pie::asio::io_service & get_io_service(pie::asio::net::socket const & s)
    {
        return s.m_io_service;
    }
}
}
}
