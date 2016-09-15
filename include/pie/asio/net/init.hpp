#pragma once

#include <Windows.h>
#include <Winsock2.h>


namespace pie {
namespace asio {
namespace net {

	static bool const initialize()
	{
		WSADATA data{};
		
		return ::WSAStartup(0x0202, &data) == 0;
	}

	static void finalize()
	{
		WSACleanup();
	}
}
}
}
