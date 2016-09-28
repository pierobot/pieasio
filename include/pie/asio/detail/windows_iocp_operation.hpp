#pragma once

#include <string>
#include <memory>

#include <Windows.h>
#include <Winsock2.h>

namespace pie
{
    namespace asio
    {
        namespace detail
        {
            struct io_operation_data
            {
                OVERLAPPED ov;
                WSABUF wsabuf;

                io_operation_data() noexcept :
                    ov(),
                    wsabuf()
                {
                }

                virtual ~io_operation_data()
                {
                }
            };
        }
    }
}
