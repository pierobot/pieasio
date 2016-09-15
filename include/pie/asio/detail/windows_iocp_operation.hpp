#pragma once

#include <pie/string.hpp>
#include <pie/memory.hpp>

#include <Windows.h>

namespace pie
{
	namespace asio
	{
		namespace detail
		{
			struct io_operation_data
			{
				OVERLAPPED ov;

				io_operation_data() :
					ov()
				{
				}

				virtual ~io_operation_data()
				{
				}
			};
		}
	}
}