#pragma once

#include <type_traits>

namespace pie
{
    namespace asio
    {
        namespace detail
        {
            struct associable
            {
            };
        }

        template<class NativeHandleType>
        class associable_object : public detail::associable
        {
        public:
            typedef NativeHandleType native_handle_type;

            virtual native_handle_type get_handle() const noexcept = 0;
        protected:
        private:
        };

        template<class Object>
        struct is_associable : std::is_base_of<detail::associable, Object>
        {
        };
    }
}
