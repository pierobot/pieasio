#pragma once

#include <atomic>

#include <pie/asio/associable.hpp>

#ifdef _WIN32
#   include <pie/asio/detail/windows_io_completion_port.hpp>
#endif

namespace pie
{
    namespace asio
    {
        class io_service
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

            template<class AssociableObject>
            bool associate(AssociableObject const & object, std::error_code & ec)
            {
                static_assert(pie::asio::is_associable<AssociableObject>::value, "Invalid object parameter. It must inherit and implement pie::asio::associable_object.");

                return m_iocp.associate(reinterpret_cast<native_handle_type>(object.get_handle()), ec);
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
            std::atomic<bool> m_continue;
        };
    }
}
