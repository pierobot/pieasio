#pragma once

#include <pie/asio/io_operation.hpp>
#include <pie/asio/net/socket.hpp>
#include <pie/asio/net/resolver.hpp>

#include <system_error>

#include <MSWSock.h>

namespace pie {
namespace asio {
namespace net {
namespace detail {

    // Initialized on first call the function below
    static LPFN_CONNECTEX ConnectEx = nullptr;
#pragma warning(push)
#pragma warning(disable: 4996)

    bool connect(pie::asio::net::socket const & socket,
                 pie::asio::net::resolver const & resolver,
                 pie::asio::io_operation_data::on_connect_type && on_connect,
                 std::error_code & ec)
    {
        // Initialize the function pointer if not done so already
        if (ConnectEx == nullptr)
        {
            GUID function_guid = WSAID_CONNECTEX;
            DWORD bytes = 0;
            int result = ::WSAIoctl(socket.get_handle(), SIO_GET_EXTENSION_FUNCTION_POINTER,
                                   &function_guid, sizeof(function_guid),
                                   &ConnectEx, sizeof(ConnectEx),
                                   &bytes, nullptr, nullptr);
            if (result == SOCKET_ERROR)
            {
                ec = std::error_code(::WSAGetLastError(), std::system_category());
                return false;
            }
        }

        if (resolver.valid() == true)
        {
            auto io_data_ptr = pie::asio::io_operation_data::create(IO_CONNECT);
            if (io_data_ptr == nullptr)
            {
                ec = std::make_error_code(std::errc::not_enough_memory);
                return false;
            }

            // Assign the on connect handler
            io_data_ptr->on_connect = std::move(on_connect);

            sockaddr_in addr_in{};
            addr_in.sin_port = ::htons(resolver.get_port());
            addr_in.sin_family = socket.get_family();
            addr_in.sin_addr.S_un.S_addr = INADDR_ANY;
            // ConnectEx requires the socket to be bound to INADDR_ANY
            if (::bind(socket.get_handle(), reinterpret_cast<sockaddr*>(&addr_in), sizeof(sockaddr)) != 0)
            {
                ec = std::error_code(::WSAGetLastError(), std::system_category());
                return false;
            }

            addr_in.sin_addr.S_un.S_addr = resolver.get_addr();

            BOOL result = ConnectEx(socket.get_handle(), reinterpret_cast<sockaddr*>(&addr_in), sizeof(sockaddr), nullptr, 0, nullptr, &io_data_ptr->ov);
            // The call to ConnectEx can succeed in the following two ways
            // 1. Return value is TRUE
            // 2. Return value is FALSE and WSAGetLastError() == ERROR_IO_PENDING
            if (result == TRUE)
            {
                ec = std::error_code();
                io_data_ptr.release();
                return true;
            }

            if (::WSAGetLastError() == ERROR_IO_PENDING)
            {
                ec = std::make_error_code(std::errc::operation_in_progress);

                io_data_ptr.release();
                return true;
            }
        }
        else
        {
            ec = std::make_error_code(std::errc::not_enough_memory);
        }

        return false;
    }

#pragma warning(pop)
}
}
}
}