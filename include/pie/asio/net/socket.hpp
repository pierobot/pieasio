#pragma once

#ifdef _WIN32
#	include <pie/asio/net/detail/windows_basic_socket.hpp>
#else
#endif

namespace pie {
namespace asio {
namespace net {

	typedef pie::asio::net::detail::basic_socket socket;
}
}
}
