#pragma once

#include <pie/asio/io_operation.hpp>
#include <pie/asio/noncopyable.hpp>

#include <functional>
#include <memory>
#include <mutex>
#include <queue>

namespace pie
{
    namespace asio
    {
        class operation_manager : private noncopyable
        {
        public:
            typedef std::function<void()> function_type;

            operation_manager() noexcept : 
                m_queue(),
                m_mtx()
            {
            }

            ~operation_manager()
            {
            }

            void push(function_type fn)
            {
                std::lock_guard<std::mutex> lock(m_mtx);
                m_queue.push(fn);
            }

            function_type pop()
            {
                function_type fn;
                {
                    std::lock_guard<std::mutex> lock(m_mtx);
                    if (m_queue.size() > 0)
                    {
                        fn = std::move(m_queue.front());
                        m_queue.pop();
                    }
                }
                
                return fn;
            }
        protected:
        private:
            std::queue<function_type> m_queue;
            std::mutex m_mtx;
        };
    }
}