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
			             pie::size_t size,
			             pie::asio::on_read_type && on_read,
			             pie::error_code & ec)
		{
			return detail::read(socket, size, pie::move(on_read), ec);
		}
	}
}