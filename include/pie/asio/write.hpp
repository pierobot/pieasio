#pragma once

#ifdef _WIN32
#	include <pie/asio/detail/windows_ov_write.hpp>
#else
#endif

namespace pie
{
	namespace asio
	{
		inline bool write(pie::asio::net::socket const & socket,
			              char const * buffer,
			              pie::size_t size,
			              pie::asio::on_write_type && on_write,
			              pie::error_code & ec)
		{
			return detail::write(socket, buffer, size, pie::move(on_write), ec);
		}

		inline bool write(pie::asio::net::socket const & socket,
			              pie::string const & buffer,
			              pie::asio::on_write_type && on_write,
			              pie::error_code & ec)
		{
			return detail::write(socket, buffer, pie::move(on_write), ec);
		}

		inline bool write(pie::asio::net::socket const & socket, 
			              pie::string && buffer,
			              pie::asio::on_write_type && on_write,
			              pie::error_code & ec)
		{
			return detail::write(socket, pie::move(buffer), pie::move(on_write), ec);
		}
	}
}
