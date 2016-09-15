#pragma once

#include <pie/atomic.hpp>
#include <pie/asio/detail/windows_io_completion_port.hpp>

namespace pie
{
	namespace asio
	{
		class io_service : private pie::noncopyable
		{
		public:
			typedef detail::io_completion_port<io_service> completion_port_type;
			typedef typename completion_port_type::handle_type native_handle_type;

			io_service()
			{
			}

			~io_service()
			{
			}

			template<class AssignableObject>
			bool assign(AssignableObject const & object)
			{
				return m_iocp.associate(reinterpret_cast<native_handle_type>(object.get_handle()));
			}

			void poll()
			{
				int bytes_transferred = m_iocp.poll();
				if (bytes_transferred != 0)
				{

				}
			}

			void run()
			{
				while (true)
					poll();
			}
		protected:
		private:
			completion_port_type m_iocp;
			pie::atomic<bool> m_continue;
		};
	}
}