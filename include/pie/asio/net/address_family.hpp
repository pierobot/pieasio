#pragma once

#include <Winsock2.h>
#include <Windows.h>

namespace pie
{
    namespace asio
    {
        namespace net
        {
            enum address_family
            {
                v4 = AF_INET,
                v6 = AF_INET6,
            };
        }
    }
}
