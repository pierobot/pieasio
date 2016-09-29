#pragma once

#include <string>
#include <memory>

namespace pie
{
    namespace asio
    {
        namespace detail
        {
            struct io_operation_data
            {
                int fd;

                io_operation_data() noexcept
                {
                }

                virtual ~io_operation_data()
                {
                }
            };
        }
    }
}
