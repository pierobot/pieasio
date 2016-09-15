#pragma once

#include <system_error>

#include <pie/asio/net/address_family.hpp>
#include <pie/asio/net/socket_protocol.hpp>
#include <pie/asio/net/socket_type.hpp>

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
											std::error_code & ec)
				{
					SOCKET s = ::WSASocketW(family, type, protocol, nullptr, 0, WSA_FLAG_OVERLAPPED);
					if (s == INVALID_SOCKET)
						ec = std::error_code(::WSAGetLastError(), std::system_category());

					return s;
				}

				inline void close_socket(SOCKET handle)
				{
					::closesocket(handle);
				}

				inline bool set_nonblocking(SOCKET handle, std::error_code & ec)
				{
					unsigned long arg = -1;
					if (::ioctlsocket(handle, FIONBIO, &arg) == SOCKET_ERROR)
					{
						ec = std::error_code(::WSAGetLastError(), std::system_category());
						return false;
					}

					return true;
				}
			}
		}
	}
}
