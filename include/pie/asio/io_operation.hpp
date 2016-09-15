#pragma once

#include <pie/functional.hpp>
#include <pie/system_error.hpp>

#ifdef _WIN32
#	include <pie/asio/detail/windows_iocp_operation.hpp>
#else
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
			IO_WRITE,
			IO_CLOSE
		};

		struct io_operation_data : public detail::io_operation_data
		{
			typedef pie::function<void(pie::error_code const&)> on_connect_type;
			typedef pie::function<void(pie::size_t, pie::error_code const&)> on_write_type;
			typedef pie::function<void(pie::string, pie::size_t, pie::error_code const&)> on_read_type;

			union
			{
				on_connect_type on_connect;
				on_write_type on_write;
				on_read_type on_read;
			};	

			int operation;
			pie::string buffer;

			io_operation_data(int op) :
				detail::io_operation_data(),
				on_connect(),
				on_write(),
				on_read(),
				operation(op),
				buffer()
			{
			}

			virtual ~io_operation_data()
			{
			}

			static pie::unique_ptr<io_operation_data> create(int operation)
			{
				return pie::unique_ptr<io_operation_data>(new io_operation_data(operation));
			}
		};

		using on_connect_type = io_operation_data::on_connect_type;
		using on_write_type = io_operation_data::on_write_type;
		using on_read_type = io_operation_data::on_read_type;
	}
}