#pragma once

#include <system_error>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <queue>
#include <type_traits>

#include <pie/asio/noncopyable.hpp>

namespace pie
{
    namespace asio
    {
        template<class Context>
        class context_manager : private noncopyable
        {
        public:
            static_assert(std::is_nothrow_default_constructible<Context>::value, "Type Context must be nothrow default constructible.");

            typedef Context context_type;
            typedef void * key_type;
            typedef std::shared_ptr<context_type> context_pointer_type;

            context_manager() noexcept :
                m_pending_mutex(),
                m_available_mutex(),
                m_pending(),
                m_available()
            {
            }

            ~context_manager()
            {
            }

            context_pointer_type get_free_context()
            {
                std::size_t size = 0;
                {
                    std::lock_guard<std::mutex> lock(m_available_mutex);
                    size = m_available.size();
                }
                
                // Do we have any available contexts?
                if (size == 0)
                {
                    // No, allocate and default construct one
                    context_pointer_type context_ptr(new (std::nothrow) context_pointer_type::element_type());
                    return context_ptr;
                }

                std::lock_guard<std::mutex> lock(m_available_mutex);
                context_pointer_type context_ptr = m_available.front();
                m_available.pop();

                return context_ptr;
            }

            context_pointer_type get_pending_context(key_type key)
            {
                std::lock_guard<std::mutex> lock(m_pending_mutex);
                // Search for the context based on its key
                auto iterator = m_pending.find(key);
                if (iterator != m_pending.cend())
                {
                    // Found it, move the shared_ptr and remove it from the map
                    context_pointer_type context_ptr = std::move(iterator->second);
                    m_pending.erase(iterator);

                    return context_ptr;
                }

                // Not found
                return nullptr;
            }

            void assign_free_context(context_pointer_type && context_ptr)
            {
                std::lock_guard<std::mutex> lock(m_available_mutex);
                m_available.push(std::move(context_ptr));
            }

            void assign_pending_context(context_pointer_type && context_ptr)
            {
                std::lock_guard<std::mutex> lock(m_pending_mutex);
                m_pending.emplace(context_ptr.get(), std::move(context_ptr));
            }
        protected:
        private:
            std::mutex m_pending_mutex;
            std::mutex m_available_mutex;
            std::unordered_map<key_type, context_pointer_type> m_pending;
            std::queue<context_pointer_type> m_available;
        };
    }
}