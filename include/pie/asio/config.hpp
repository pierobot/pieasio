#pragma once

#if defined(_WIN32)
#   include <Winsock2.h>
#   include <Windows.h>
#endif

namespace pie
{
    namespace asio
    {
#if defined(_WIN32)
        typedef SOCKET native_socket_type;
        typedef HANDLE native_handle_type;
#elif defined(__linux__)
        typedef int native_socket_type;
        typedef int native_handle_type;
#endif
    }
}
