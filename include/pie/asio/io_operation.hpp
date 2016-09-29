#pragma once

#include <functional>
#include <system_error>

#ifdef _WIN32
#   include <pie/asio/detail/windows_iocp_operation.hpp>
#else
#   include <pie/asio/detail/linux_operation.hpp>
#endif

namespace pie
{
    namespace asio
    {
        enum io_operation
        {
            IO_ERROR = 0,
            IO_ACCEPT,
            IO_CONNECT,
            IO_READ,
            IO_READ_UNTIL,
            IO_WRITE,
            IO_CLOSE
        };

        struct io_operation_data : public detail::io_operation_data
        {
            typedef std::function<void(std::error_code const&)> on_connect_type;
            typedef std::function<void(std::size_t, std::error_code const&)> on_write_type;
            typedef std::function<void(std::string, std::size_t, std::error_code const&)> on_read_type;

            union
            {
                on_connect_type on_connect;
                on_write_type on_write;
                on_read_type on_read;
            };  

            int operation;
            std::string buffer;

            io_operation_data() noexcept :
                detail::io_operation_data(),
                on_connect(),
                operation(IO_ERROR),
                buffer()
            {
            }

            io_operation_data(int op) noexcept :
                detail::io_operation_data(),
                on_connect(),
                operation(op),
                buffer()
            {
            }

            virtual ~io_operation_data()
            {
            }
        };

        using on_connect_type = io_operation_data::on_connect_type;
        using on_write_type = io_operation_data::on_write_type;
        using on_read_type = io_operation_data::on_read_type;
    }
}
