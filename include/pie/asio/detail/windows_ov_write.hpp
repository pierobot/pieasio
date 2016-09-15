#pragma once

#include <pie/stdint.hpp>
#include <pie/memory.hpp>
#include <pie/asio/net/socket.hpp>
#include <pie/asio/io_operation.hpp>
#include <pie/system_error.hpp>

namespace pie
{
	namespace asio
	{
		namespace detail
		{
			bool write(pie::asio::net::socket const & socket,
				       char const * buffer,
				       pie::size_t size,
				       pie::asio::on_write_type && on_write,
				       pie::error_code & ec)
			{
				auto io_data_ptr = pie::asio::io_operation_data::create(io_operation::IO_WRITE);
				if (io_data_ptr == nullptr)
				{
					ec = pie::error_code(ERROR_NOT_ENOUGH_MEMORY, pie::system_category());
				}
				else
				{
					WSABUF buf;
					buf.buf = const_cast<char *>(buffer);
					buf.len = size;

					io_data_ptr->on_write = pie::move(on_write);
					
					int result = ::WSASend(socket.get_handle(), &buf, 1, nullptr, 0, reinterpret_cast<WSAOVERLAPPED*>(&io_data_ptr->ov), nullptr);
					if (result != SOCKET_ERROR)
					{
						io_data_ptr.release();
						return true;
					}

					ec = pie::error_code(::WSAGetLastError(), pie::system_category());
					if (ec.value() == WSA_IO_PENDING)
					{
						io_data_ptr.release();
						return true;
					}
				}

				return false;
			}

			bool write(pie::asio::net::socket const & socket,
				       pie::string const & buffer,
				       pie::asio::on_write_type && on_write,
				       pie::error_code & ec)
			{
				auto io_data_ptr = pie::asio::io_operation_data::create(io_operation::IO_WRITE);
				if (io_data_ptr == nullptr)
				{
					ec = pie::error_code(ERROR_NOT_ENOUGH_MEMORY, pie::system_category());
				}
				else
				{
					io_data_ptr->buffer = buffer;
					io_data_ptr->on_write = pie::move(on_write);

					WSABUF buf;
					buf.buf = const_cast<char *>(io_data_ptr->buffer.c_str());
					buf.len = io_data_ptr->buffer.size();

					int result = ::WSASend(socket.get_handle(), &buf, 1, nullptr, 0, reinterpret_cast<WSAOVERLAPPED*>(&io_data_ptr->ov), nullptr);
					if (result != SOCKET_ERROR)
					{
						io_data_ptr.release();
						return true;
					}

					ec = pie::error_code(::WSAGetLastError(), pie::system_category());
					if (ec.value() == WSA_IO_PENDING)
					{
						io_data_ptr.release();
						return true;
					}
				}

				return false;
			}

			bool write(pie::asio::net::socket const & socket, 
				       pie::string && buffer,
				       pie::asio::on_write_type && on_write,
				       pie::error_code & ec)
			{
				auto io_data_ptr = pie::asio::io_operation_data::create(io_operation::IO_WRITE);
				if (io_data_ptr == nullptr)
				{
					ec = pie::error_code(ERROR_NOT_ENOUGH_MEMORY, pie::system_category());
				}
				else
				{
					io_data_ptr->buffer = pie::move(buffer);
					io_data_ptr->on_write = pie::move(on_write);

					WSABUF buf;
					buf.buf = const_cast<char *>(io_data_ptr->buffer.c_str());
					buf.len = io_data_ptr->buffer.size();

					int result = ::WSASend(socket.get_handle(), &buf, 1, nullptr, 0, reinterpret_cast<WSAOVERLAPPED*>(&io_data_ptr->ov), nullptr);
					if (result != SOCKET_ERROR)
					{
						io_data_ptr.release();
						return true;
					}

					ec = pie::error_code(::WSAGetLastError(), pie::system_category());
					if (ec.value() == WSA_IO_PENDING)
					{
						io_data_ptr.release();
						return true;
					}
				}

				return false;
			}
		}
	}
}
