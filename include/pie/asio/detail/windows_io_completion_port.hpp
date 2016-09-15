#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include <pie/asio/io_operation.hpp>

#include <Windows.h>

namespace pie {
namespace asio {
namespace detail {
	
	struct io_port_context
	{
		OVERLAPPED overlapped;
		HANDLE object_handle;

		io_port_context(HANDLE handle) :
			object_handle(handle),
			overlapped{}
		{
		}
	};

	template<class Owner>
	class io_completion_port
	{
	public:
		typedef HANDLE handle_type;
		typedef std::unique_ptr<io_port_context> context_pointer;

		io_completion_port() noexcept :
			m_handle(::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0))
		{
		}

		virtual ~io_completion_port()
		{
			::CloseHandle(m_handle);
		}

		bool associate(HANDLE object_handle)
		{
			// Create our internal context
			context_pointer context_ptr(new io_port_context(object_handle));
			if (context_ptr != nullptr)
			{
				context_ptr->object_handle = object_handle;
				// Associate the object and custom context with our completion port
				if (::CreateIoCompletionPort(object_handle, m_handle, reinterpret_cast<ULONG_PTR>(context_ptr->object_handle), 0) == m_handle)
				{
					std::lock_guard<pie::mutex> lock(m_mutex);
					
					m_contexts.emplace_back(std::move(context_ptr));
					
					return true;
				}
				else
				{
					int i = GetLastError();
					++i;
				}
			}
			
			return false;
		}

		DWORD poll() const
		{
			DWORD bytes_transferred = 0;
			ULONG_PTR key = 0;
			LPOVERLAPPED overlapped = nullptr;
			
			BOOL success = ::GetQueuedCompletionStatus(m_handle, &bytes_transferred, &key, &overlapped, 250);
			if (success == TRUE)
			{
				handle_operation(overlapped, bytes_transferred);
			}
			else
			{
				int last_error = ::WSAGetLastError();
				// Was the completion port closed?
				if (overlapped == nullptr && last_error == ERROR_ABANDONED_WAIT_0)
				{
					// Yes, do cleanup
					int x = 0;
				}
			}

			return bytes_transferred;
		}

		void post() const
		{
		}
	protected:
	private:
		HANDLE m_handle;
		std::vector<context_pointer> m_contexts;
		std::mutex m_mutex;

		void handle_operation(LPOVERLAPPED overlapped, DWORD bytes_transferred) const
		{
			if (overlapped != nullptr)
			{
				pie::asio::io_operation_data * io_data = CONTAINING_RECORD(overlapped, pie::asio::io_operation_data, ov);

				switch (io_data->operation)
				{
				case io_operation::IO_CONNECT:
				{
					if (io_data->on_connect != nullptr)
					{
                        std::error_code ec;
						io_data->on_connect(ec);
					}
				}
				break;

				case io_operation::IO_WRITE:
				{
					if (io_data->on_write != nullptr)
					{
                        std::error_code ec;
						io_data->on_write(static_cast<std::size_t>(bytes_transferred), ec);
					}
				}
				break;

				case io_operation::IO_READ:
					break;
				default:
					break;
				}
			}
		}
	};
}
}
}
