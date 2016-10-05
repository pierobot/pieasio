#pragma once

#if defined(_WIN32)
#   include <pie/asio/net/detail/windows_resolver.hpp>
#elif defined(__linux__)
#   include <pie/asio/net/detail/linux_resolver.hpp>
#endif
