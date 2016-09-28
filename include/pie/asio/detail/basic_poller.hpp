#pragma once

#include <memory>
#include <queue>

#include <pie/asio/io_operation.hpp>
#include <pie/asio/context_manager.hpp>
#include <pie/asio/operation_manager.hpp>

namespace pie
{
    namespace asio
    {
        namespace detail
        {
            class basic_poller
            {
            public:
                typedef context_manager::context_pointer_type context_pointer_type;

                basic_poller() noexcept :
                    m_context_manager(),
                    m_operation_manager()
                {
                }

                virtual ~basic_poller() 
                {
                }

                virtual std::size_t poll() = 0;

                context_manager & get_context_manager() noexcept
                {
                    return m_context_manager;
                }

                operation_manager & get_operation_manager() noexcept
                {
                    return m_operation_manager;
                }
            protected:
            private:
                context_manager m_context_manager;
                operation_manager m_operation_manager;
            };
        }
    }
}