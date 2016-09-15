#pragma once

#include <pie/asio/net/detail/winsock_wrapper.hpp>
#include <pie/asio/assignable_object.hpp>
#include <pie/system_error.hpp>

#include <pie/string.hpp>

namespace pie {
namespace asio {
namespace net {
namespace detail
{
	class basic_socket
	{
	public:
		typedef SOCKET native_handle;
		
		basic_socket() noexcept :
			m_handle(INVALID_SOCKET)
		{
		}
		
		basic_socket(pie::asio::net::address_family family, pie::error_code ec = pie::error_code()) noexcept :
			m_family(family),
			m_handle(pie::asio::net::detail::create_socket(family, pie::asio::net::socket_type::stream, pie::asio::net::socket_protocol::tcp, ec))
		{
		}

		virtual ~basic_socket()
		{
			if (m_handle != INVALID_SOCKET)
				pie::asio::net::detail::close_socket(m_handle);
		}
		
		native_handle get_handle() const noexcept
		{
			return m_handle;
		}

		pie::asio::net::address_family get_family() const noexcept
		{
			return m_family;
		}

		bool set_nonblocking(pie::error_code & ec = pie::error_code())
		{
			if (detail::set_nonblocking(m_handle, ec) == false)
			{
				m_handle = INVALID_SOCKET;
				ec = pie::error_code(::WSAGetLastError(), pie::system_category());
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