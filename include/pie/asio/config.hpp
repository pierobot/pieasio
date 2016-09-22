#pragma once

#ifdef _WIN32
#include <Winsock2.h>
#include <Windows.h>
#else
#endif

namespace pie
{
    namespace asio
    {
#ifdef _WIN32
        typedef SOCKET native_socket_type;
        typedef HANDLE native_handle_type;
#else
#endif
    }
}
