#pragma once

#include <pie/functional.hpp>
#include <pie/string.hpp>
#include <pie/stdint.hpp>
#include <pie/system_error.hpp>
#include <pie/asio/net/socket.hpp>
#include <pie/asio/net/resolver.hpp>

#ifdef _WIN32
#	include <pie/asio/net/detail/windows_connect.hpp>
#else
#endif

namespace pie {
namespace asio {
namespace net {
	
	bool connect(pie::asio::net::socket const & socket,
	  			 pie::asio::net::resolver const & resolver,
				 pie::asio::io_operation_data::on_connect_type && on_connect,
				 pie::error_code & ec) noexcept
	{
		return detail::connect(socket, resolver, pie::move(on_connect), ec);
	}

}
}
}
