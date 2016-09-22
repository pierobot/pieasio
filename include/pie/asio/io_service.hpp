#pragma once

#include <atomic>

#include <pie/asio/associable.hpp>
#include <pie/asio/noncopyable.hpp>
#include <pie/asio/context_manager.hpp>

#ifdef _WIN32
#   include <pie/asio/detail/windows_io_completion_port.hpp>
#endif

namespace pie
{
    namespace asio
    {
        class io_service : private noncopyable
        {
        public:
            typedef pie::asio::context_manager<pie::asio::io_operation_data> context_manager_type;
            typedef context_manager_type::context_pointer_type context_pointer_type;
#ifdef _WIN32
            typedef pie::asio::detail::io_completion_port<io_service> completion_port_type;
#endif


            io_service() : 
                m_context_manager(),
                m_iocp(m_context_manager)
            {
            }

            ~io_service()
            {
            }

            template<class AssociableObject>
            bool associate(AssociableObject const & object, std::error_code & ec = std::error_code())
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
            context_manager_type m_context_manager;
            completion_port_type m_iocp;
            std::atomic<bool> m_continue;

            friend auto get_context_manager(pie::asio::io_service & service) noexcept -> decltype(service.m_context_manager) &;
        };

        auto get_context_manager(pie::asio::io_service & service) noexcept -> decltype(service.m_context_manager) &
        {
            return service.m_context_manager;
        }
    }
}
