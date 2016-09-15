#pragma once

#include <pie/type_traits/enable_if.hpp>
#include <pie/type_traits/is_base_of.hpp>

namespace pie
{
	namespace asio
	{
		namespace iptcp
		{
			template<class Socket>
			struct socket_traits;

			template<class Socket, class = typename pie::enable_if<pie::is_base_of<pie::asio::detail::basic_socket, Socket>::value>::type>
			struct socket_traits
			{
				typedef Socket socket_type;
				typedef typename Socket::native_handle native_handle_type;
			};
		}
	}
}
