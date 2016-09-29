#pragma once

#include <atomic>

#include <pie/asio/associable.hpp>
#include <pie/asio/noncopyable.hpp>

#if defined(_WIN32)
#   if defined(PIE_ASIO_HAS_IOCP)
#       include <pie/asio/detail/windows_io_completion_port.hpp>
#   endif
#elif defined(__linux__)
#   if defined(PIE_ASIO_HAS_EPOLL)
#	    include <pie/asio/detail/linux_epoll.hpp>
#   endif
#endif

namespace pie
{
    namespace asio
    {
        class io_service : private noncopyable
        {
        public:
#if defined(_WIN32)
            typedef pie::asio::detail::io_completion_port io_poller_type;
#elif defined(__linux__)
            typedef pie::asio::detail::io_epoll io_poller_type;
#endif


            io_service() :
                m_poller()
            {
            }

            ~io_service()
            {
            }

            template<class AssociableObject>
            bool associate(AssociableObject const & object, std::error_code & ec = std::error_code())
            {
                static_assert(pie::asio::is_associable<AssociableObject>::value, "Invalid object parameter. It must inherit and implement pie::asio::associable_object.");

                return m_poller.associate(reinterpret_cast<native_handle_type>(object.get_handle()), ec);
            }

            void poll()
            {
                int bytes_transferred = m_poller.poll();
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
            io_poller_type m_poller;
            std::atomic<bool> m_continue;
            friend auto get_context_manager(pie::asio::io_service & service) noexcept -> decltype(m_poller.get_context_manager()) &;
        };

        auto get_context_manager(pie::asio::io_service & service) noexcept -> decltype(service.m_poller.get_context_manager()) &
        {
            return service.m_poller.get_context_manager();
        }
    }
}
