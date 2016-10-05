#pragma once

#if defined(_WIN32)
#    include <Winsock2.h>
#    include <Windows.h>
#elif defined(__linux__)
#    include <sys/socket.h>
#endif

namespace pie
{
    namespace asio
    {
        namespace net
        {
            enum socket_type
            {
                stream = SOCK_STREAM,
                datagram = SOCK_DGRAM
            };
        }
    }
}

