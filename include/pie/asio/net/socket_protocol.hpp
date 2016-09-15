#pragma once

#include <Winsock2.h>
#include <Windows.h>

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
