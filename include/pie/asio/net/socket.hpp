#pragma once

#ifdef _WIN32
#   include <pie/asio/net/detail/windows_basic_socket.hpp>
#else
#endif

#include <pie/asio/io_service.hpp>

namespace pie {
namespace asio {
namespace net {

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

        socket(pie::asio::io_service & io_service, pie::asio::net::address_family family, std::error_code & ec = std::error_code()) noexcept :
            detail::basic_socket(family, ec),
            m_io_service(io_service)
        {

        }
	protected:
	private:
        pie::asio::io_service & m_io_service;

        friend auto get_io_service(pie::asio::net::socket const & s) -> decltype(s.m_io_service) &;
	};

    auto get_io_service(pie::asio::net::socket const & s) -> decltype(s.m_io_service) &
    {
        return s.m_io_service;
    }
}
}
}
