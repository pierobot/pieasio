#pragma once

#include <Winsock2.h>
#include <Windows.h>

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
