#pragma once

#include <pie/asio/net/address_family.hpp>
#include <pie/asio/net/socket_protocol.hpp>
#include <pie/asio/net/socket_type.hpp>
#include <pie/system_error.hpp>

namespace pie
{
	namespace asio
	{
		namespace net
		{
			namespace detail
			{
				inline SOCKET create_socket(pie::asio::net::address_family family,
					                        pie::asio::net::socket_type type,
					                        pie::asio::net::socket_protocol protocol,
											pie::error_code & ec)
				{
					SOCKET s = ::WSASocketW(family, type, protocol, nullptr, 0, WSA_FLAG_OVERLAPPED);
					if (s == INVALID_SOCKET)
						ec = pie::error_code(::WSAGetLastError(), pie::system_category());

					return s;
				}

				inline void close_socket(SOCKET handle)
				{
					::closesocket(handle);
				}

				inline bool set_nonblocking(SOCKET handle, pie::error_code & ec)
				{
					unsigned long arg = -1;
					if (::ioctlsocket(handle, FIONBIO, &arg) == SOCKET_ERROR)
					{
						ec = pie::error_code(::WSAGetLastError(), pie::system_category());
						return false;
					}

					return true;
				}
			}
		}
	}
}
