#pragma once

#include <sys/epoll.h>
#include <unistd.h>

#include <pie/asio/config.hpp>
#include <pie/asio/get_error.hpp>
#include <pie/asio/detail/basic_poller.hpp>

namespace pie
{
    namespace asio
    {
        namespace detail
        {
            class io_epoll : public basic_poller
            {
            public:
                io_epoll() noexcept :
                    basic_poller(),
                    m_epfd(::epoll_create(32))
                {
                    if (m_epfd == -1)
                        throw std::system_error(std::error_code(errno, std::system_category()), "epoll_create");
                }

                ~io_epoll()
                {
                    if (m_epfd != ~0)
                        ::close(m_epfd);
                }

                bool associate(native_handle_type object_handle, std::error_code & ec) noexcept
                {
                    epoll_event event {};
                    // Indicate that we are interested in edge-triggered, input, output, and errors
                    event.events = EPOLLET | EPOLLIN | EPOLLOUT | EPOLLERR;
                    // Will need this to know which handle an event belongs to
                    event.data.fd = object_handle;
                    // Associate the object with our epoll fd
                    return get_errno_error(::epoll_ctl(m_epfd, EPOLL_CTL_ADD, object_handle, &event), ec) == 0;
                }

                bool disassociate(native_handle_type object_handle, std::error_code & ec) noexcept
                {
                    // Disassociate the object from our epoll fd
                    return get_errno_error(::epoll_ctl(m_epfd, EPOLL_CTL_DEL, object_handle, nullptr), ec) == 0;
                }

                std::size_t poll() noexcept
                {
                    // Why 128? No idea, that's just the value in asio's epoll_reactor.ipp
                    // asio is a mature implementation, so let's go with this for now
                    epoll_event events[128];
                    std::error_code poll_error;

                    int events_ready = get_errno_error(::epoll_wait(m_epfd, &events[0], 128, 50), poll_error);
                    if (poll_error)
                        return 0;

                    std::size_t events_processed = 0;
                    for (int i = 0; i < events_ready; ++i)
                    {
                        auto io_data_ptr = get_context_manager().get_pending_context(events[i].data.ptr);
                        if (io_data_ptr != nullptr)
                        {
                            get_operation_manager().push([=]() mutable
                            {
                                process_io(std::move(io_data_ptr));
                            });

                            ++events_processed;
                        }
                    }

                    return events_processed;
                }
            protected:
            private:
                int m_epfd;

                void process_io(std::shared_ptr<pie::asio::io_operation_data> io_data_ptr)
                {
                    if (io_data_ptr == nullptr)
                        return;

                    switch (io_data_ptr->operation)
                    {
                    case io_operation::IO_CONNECT:
                    {
                        if (io_data_ptr->on_connect != nullptr)
                            io_data_ptr->on_connect(std::error_code());
                    }
                        break;

                    case io_operation::IO_READ:
                    {
                    }
                        break;

                    case io_operation::IO_WRITE:
                    {
                    }
                        break;

                    default:
                        break;
                    }

                    get_context_manager().assign_free_context(std::move(io_data_ptr));
                }
            };
             }
    }
}
