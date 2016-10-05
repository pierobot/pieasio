#pragma once

#include <pie/asio/net/socket.hpp>
#include <pie/asio/net/resolver.hpp>

#if defined(_WIN32)
#   include <pie/asio/net/detail/windows_connect.hpp>
#elif defined(__linux__)
#   include <pie/asio/net/detail/linux_connect.hpp>
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
                 std::error_code & ec)
    {
        return detail::connect(socket, resolver, std::move(on_connect), ec);
    }

}
}
}
