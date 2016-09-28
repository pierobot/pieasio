#pragma once

#if defined(_WIN32)
#   include <Windows.h>
#   include <Winsock2.h>
#endif

namespace pie {
namespace asio {
namespace net {

    static bool const initialize()
    {
#if defined(_WIN32)
        WSADATA data{};
        
        return ::WSAStartup(0x0202, &data) == 0;
#endif
        return true;
    }

    static void finalize()
    {
#if defined(_WIN32)
        WSACleanup();
#endif
    }
}
}
}
