#include <pie/asio/asio.hpp>

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

pie::asio::io_service g_service;
std::atomic<bool> g_is_running = true;

TEST_CASE("sockets")
{
    SECTION("v4 asynchronous")
    {
        std::error_code socket_error;
        std::error_code connect_error;
        std::error_code write_error;
        std::error_code read_error;
        pie::asio::net::socket socket(g_service, pie::asio::net::v4);
        pie::asio::net::resolver resolver("pie-vm-01.cloudapp.net", 80);
        std::atomic<bool> connect_done(false);
        std::atomic<bool> write_done(false);
        std::atomic<bool> read_done(false);
        bool connect_result = false;
        bool write_result = false;
        bool read_result = false;
        std::size_t total_written = 0;
        std::string request_str("GET / HTTP/1.1\r\n"
                                "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2272.118 Safari/537.36\r\n"
                                "Host: pie-vm-01.cloudapp.net\r\n"
                                "Connection: close\r\n\r\n");
        std::string response_str;
        g_service.associate(socket);

        connect_result = pie::asio::net::connect(socket, resolver,
            [&](std::error_code const & error)
        {
            connect_error = error;
            connect_done = true;
        }, connect_error);

        REQUIRE(connect_result == true);
        if (connect_error)
            REQUIRE(connect_error == std::make_error_code(std::errc::operation_in_progress));

        while (!connect_done)
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

        REQUIRE(connect_error.value() == 0);

        write_result = pie::asio::write(socket, request_str,
            [&](std::size_t bytes_written, std::error_code const & error)
        {
            write_error = error;
            total_written += bytes_written;
            write_done = true;

        }, write_error);

        REQUIRE(write_result == true);
        if (write_error)
            REQUIRE(write_error == std::make_error_code(std::errc::operation_in_progress));

        while (!write_done)
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

        REQUIRE(write_error.value() == 0);
        REQUIRE(total_written == request_str.length());

        read_result = pie::asio::read(socket, 15,
            [&](std::string contents, std::size_t bytes_read, std::error_code const & error)
        {
            read_error = error;
            response_str = std::move(contents);
            read_done = true;
        }, read_error);

        REQUIRE(read_result == true);
        if (read_error)
            REQUIRE(read_error == std::make_error_code(std::errc::operation_in_progress));

        while (!read_done)
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

        REQUIRE(read_error.value() == 0);
        REQUIRE(response_str == "HTTP/1.1 200 OK");
    }
}

int main(int argc, char ** argv)
{
    pie::asio::net::initialize();

    int result = 0;
    {
        std::thread worker([]()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            while (g_is_running)
                g_service.poll();
        });

        result = Catch::Session().run(argc, argv);

        g_is_running = false;
        worker.join();
    }
    
    pie::asio::net::finalize();

    return result;
}
