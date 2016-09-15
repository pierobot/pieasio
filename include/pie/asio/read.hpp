#pragma once

#ifdef _WIN32
#include <pie/asio/detail/windows_ov_read.hpp>
#else
#endif

namespace pie
{
	namespace asio
	{
		inline bool read(pie::asio::net::socket const & socket,
			             std::size_t size,
			             pie::asio::on_read_type && on_read,
			             std::error_code & ec)
		{
			return detail::read(socket, size, std::move(on_read), ec);
		}
	}
}