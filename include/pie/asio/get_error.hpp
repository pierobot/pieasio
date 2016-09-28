#pragma once

#include <system_error>

#if defined(_WIN32)
#   include <Windows.h>
#elif defined(__linux__)
#   include <errno.h>
#endif

namespace pie
{
    namespace asio
    {
#if defined(_WIN32)
        template<class T>
        T get_wsa_error(T result, std::error_code & ec) noexcept
        {
            if (result == ~0)
                ec = std::error_code(::WSAGetLastError(), std::system_category());
            else
                ec = std::error_code();
            
            return result;
        }

        template<class T>
        T get_win_error(T result, std::error_code & ec) noexcept
        {
            if (result == ~0)
                ec = std::error_code(::GetLastError(), std::system_category());
            else
                ec = std::error_code();
            
            return result;
        }
#elif defined(__linux__)
        template<class T>
        T get_errno_error(T result, std::error_code & ec) noexcept
        {
            if (result == ~0)
                ec = std::error_code(errno, std::system_category());
            else
                ec = std::error_code();
            
            return result;
        }
#endif
    }
}
