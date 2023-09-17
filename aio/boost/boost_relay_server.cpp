//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2022 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <atomic>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
// #define BOOST_ASIO_HAS_IO_URING
// #define BOOST_ASIO_DISABLE_EPOLL

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace {

size_t                      connection_count;
std::atomic<size_t>         closed_connection_count{0};
tcp::resolver::results_type endpoints;

void connection_closed()
{
    if (connection_count && closed_connection_count.fetch_add(1) == (connection_count - 1)) {
        std::cout << "Exiting" << std::endl;
        exit(0);
    }
}

} // namespace

class session
    : public std::enable_shared_from_this<session> {
public:
    session(tcp::socket socket, boost::asio::io_context& _ctx)
        : socket1_(std::move(socket))
        , socket2_(_ctx)
    {
    }

    void start()
    {
        do_connect();
    }

private:
    void do_read1()
    {
        socket1_.async_read_some(boost::asio::buffer(data1_, max_length),
            [this, self = shared_from_this()](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    do_write2(length);
                } else {
                    connection_closed();
                }
            });
    }

    void do_read2()
    {
        socket2_.async_read_some(boost::asio::buffer(data2_, max_length),
            [this, self = shared_from_this()](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    do_write1(length);
                }
            });
    }

    void do_write1(std::size_t length)
    {
        boost::asio::async_write(socket1_, boost::asio::buffer(data2_, length),
            [this, self = shared_from_this()](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    do_read2();
                } else {
                }
            });
    }

    void do_write2(std::size_t length)
    {
        boost::asio::async_write(socket2_, boost::asio::buffer(data1_, length),
            [this, self = shared_from_this()](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    do_read1();
                } else {
                }
            });
    }

    void do_connect()
    {
        boost::asio::async_connect(socket2_, endpoints,
            [this, self = shared_from_this()](boost::system::error_code ec, tcp::endpoint) {
                if (!ec) {
                    do_read1();
                    do_read2();
                }
            });
    }

    tcp::socket socket1_;
    tcp::socket socket2_;
    enum { max_length = 1024 * 4 };
    char data1_[max_length];
    char data2_[max_length];
};

class server {
public:
    server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
        , ctx_(io_context)
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    socket.non_blocking(true);
                    std::make_shared<session>(std::move(socket), ctx_)->start();
                }

                do_accept();
            });
    }

    tcp::acceptor            acceptor_;
    boost::asio::io_context& ctx_;
};

int main(int argc, char* argv[])
{
    try {
        std::string_view other_port = "2000";

        if (argc < 2) {
            std::cerr << "Usage: async_tcp_echo_server <port> <echo-port> <connection_count>\n";
            return 1;
        }

        if (argc > 2) {
            other_port = argv[2];
        }

        if (argc > 3) {
            connection_count = atoi(argv[3]);
            std::cout << "Connection count " << connection_count << std::endl;
        }

        boost::asio::io_context        io_context;
        boost::asio::ip::tcp::resolver resolver(io_context);

        endpoints = resolver.resolve("localhost", other_port);

        server s(io_context, std::atoi(argv[1]));

        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}