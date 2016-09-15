#pragma once

#include <pie/asio/net/socket.hpp>
#include <pie/asio/net/resolver.hpp>

#ifdef _WIN32
#	include <pie/asio/net/detail/windows_connect.hpp>
#else
#endif

#include <functional>
#include <string>
#include <system_error>

namespace pie {
namespace asio {
namespace net {
	
	bool connect(pie::asio::net::socket const & socket,
	  			 pie::asio::net::resolver const & resolver,
				 pie::asio::io_operation_data::on_connect_type && on_connect,
				 std::error_code & ec) noexcept
	{
		return detail::connect(socket, resolver, std::move(on_connect), ec);
	}

}
}
}
