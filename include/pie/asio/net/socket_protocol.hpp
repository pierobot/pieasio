#pragma once

#if defined(_WIN32)
#    include <Winsock2.h>
#    include <Windows.h>
#elif defined(__linux__)
#    include <sys/types.h>
#    include <netinet/in.h>
#endif

namespace pie
{
    namespace asio
    {
        namespace net
        {
            enum socket_protocol
            {
                tcp = IPPROTO_TCP,
                udp = IPPROTO_UDP,
            };
        }
    }
}
