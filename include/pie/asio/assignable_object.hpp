#pragma once

namespace pie
{
	namespace asio
	{
		template<class NativeHandleType>
		class assignable_object
		{
		public:
			typedef NativeHandleType native_handle_type;

			virtual native_handle_type get_handle() const = 0;
		protected:
		private:
		};
	}
}
