#pragma once

#if defined(_WIN32)
#   include <pie/asio/detail/windows_ov_write.hpp>
#elif defined(__linux__)
#   include <pie/asio/detail/linux_async_write.hpp>
#endif

namespace pie
{
    namespace asio
    {
        inline bool write(pie::asio::net::socket const & socket,
                          char const * buffer,
                          std::size_t size,
                          pie::asio::on_write_type && on_write,
                          std::error_code & ec)
        {
            return detail::write(socket, buffer, size, std::move(on_write), ec);
        }

        inline bool write(pie::asio::net::socket const & socket,
                          std::string const & buffer,
                          pie::asio::on_write_type && on_write,
                          std::error_code & ec)
        {
            return detail::write(socket, buffer, std::move(on_write), ec);
        }

        inline bool write(pie::asio::net::socket const & socket,
                          std::string && buffer,
                          pie::asio::on_write_type && on_write,
                          std::error_code & ec)
        {
            return detail::write(socket, std::move(buffer), std::move(on_write), ec);
        }
    }
}

