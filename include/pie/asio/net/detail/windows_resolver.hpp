#pragma once

#include <string>

#include <WinSock2.h>

namespace pie
{
	namespace asio
	{
		namespace net
		{
			class resolver
			{
			public:
				resolver(std::string const & hostname, uint16_t port) noexcept :
					m_hostent(::gethostbyname(hostname.c_str())),
					m_port(port)
				{
				}

				~resolver()
				{
				}

				uint16_t get_port() const noexcept
				{
					return m_port;
				}

				unsigned long get_addr() const
				{
					return *reinterpret_cast<unsigned long *>(m_hostent->h_addr_list[0]);
				}

				bool valid() const noexcept
				{
					return m_hostent != nullptr;
				}
			protected:
			private:
				uint16_t m_port;
				hostent * m_hostent;
			};
		}
	}
}
#pragma 