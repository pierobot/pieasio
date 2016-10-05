#pragma once

namespace pie
{
    namespace asio
    {
        class noncopyable
        {
        protected:
            noncopyable() {}
            ~noncopyable() {}
        //private:
            noncopyable(noncopyable const&) = delete;
            noncopyable const & operator=(noncopyable const&) = delete;
        };
    }

    using asio::noncopyable;
}
