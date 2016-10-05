#pragma once

#if defined(_WIN32)
#   include <Winsock2.h>
#elif defined(__linux__)
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <netdb.h>
#endif

#include <cstdint>
#include <iterator>
#include <string>
#include <system_error>

#include <pie/asio/get_error.hpp>

namespace pie
{
    namespace asio
    {
        namespace net
        {
            class address_iterator :
                public std::iterator<std::forward_iterator_tag, addrinfo>
            {
            public:
                address_iterator() noexcept :
                    m_addr(nullptr)
                {
                }

                address_iterator(addrinfo * addr) noexcept :
                    m_addr(addr)
                {
                }

                address_iterator & operator++() noexcept
                {
                    // Start out with an empty iterator in case of errors
                    static address_iterator it = address_iterator();
                    // Is this an empty iterator?
                    if (m_addr == nullptr)
                        // Yes, return an empty iterator
                        return it;
                    // Is there another address info structure?
                    if (m_addr->ai_next == nullptr)
                        // No
                        return it;

                    m_addr = m_addr->ai_next;

                    return *this;
                }

                address_iterator operator++(int) noexcept
                {
                    address_iterator it = *this;
                    ++it;

                    return it;
                }

                pointer operator*() noexcept
                {
                    // Esnure we don't cause an access violation
                    if (m_addr == nullptr)
                        return nullptr;

                    return m_addr;
                }

                bool operator==(address_iterator const & other) const noexcept
                {
                    return m_addr == other.m_addr;
                }

                bool operator!=(address_iterator const & other) const noexcept
                {
                    return !(*this == other);
                }
            protected:
            private:
                addrinfo * m_addr;
            };

            class resolver
            {
            public:
                typedef address_iterator iterator;

                resolver(std::string hostname, uint16_t port, std::error_code & ec) noexcept :
                    m_hostname(hostname),
                    m_port(port),
                    m_addrinfo(nullptr)
                {
                    std::string port_str = std::to_string(port);
#if defined(_WIN32)
                    get_wsa_error(::getaddrinfo(nullptr, port_str.c_str(), nullptr, &m_addrinfo), ec);
#elif defined(__linux__)
                    get_errno_error(::getaddrinfo(nullptr, port_str.c_str(), nullptr, &m_addrinfo), ec);
#endif
                }

                ~resolver()
                {
                    if (m_addrinfo != nullptr)
                        ::freeaddrinfo(m_addrinfo);
                }

                iterator begin() const noexcept
                {
                    return iterator(m_addrinfo);
                }

                iterator end() const noexcept
                {
                    return iterator();
                }

                bool const valid() const noexcept
                {
                    return m_addrinfo != nullptr;
                }
            protected:
            private:
                std::string m_hostname;
                uint16_t m_port;
                addrinfo * m_addrinfo;
            };
        }
    }
}

