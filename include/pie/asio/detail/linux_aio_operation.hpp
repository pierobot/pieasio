#pragma once

#include <string>
#include <memory>

#include <aio.h>

namespace pie
{
    namespace asio
    {
        namespace detail
        {
            struct io_operation_data
            {
                int fd;
                aiocb * aio_data;

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
