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
//#define BOOST_ASIO_HAS_IO_URING
//#define BOOST_ASIO_DISABLE_EPOLL

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace {

size_t              connection_count;
std::atomic<size_t> closed_connection_count{0};

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
    session(tcp::socket socket)
        : socket_(std::move(socket))
    {
    }

    void start()
    {
        do_read();
    }

private:
    void do_read()
    {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            [this, self = shared_from_this()](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    do_write(length);
                } else {
                    connection_closed();
                }
            });
    }

    void do_write(std::size_t length)
    {
        boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
            [this, self = shared_from_this()](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    do_read();
                } else {
                    connection_closed();
                }
            });
    }

    tcp::socket socket_;
    enum { max_length = 1024 * 4 };
    char data_[max_length];
};

class server {
public:
    server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
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
                    std::make_shared<session>(std::move(socket))->start();
                }

                do_accept();
            });
    }

    tcp::acceptor acceptor_;
};

int main(int argc, char* argv[])
{
    try {
        if (argc < 2) {
            std::cerr << "Usage: async_tcp_echo_server <port> <connection_count>\n";
            return 1;
        }

        if (argc > 2) {
            connection_count = atoi(argv[2]);
            std::cout << "Connection count " << connection_count << std::endl;
        }

        boost::asio::io_context io_context;

        server s(io_context, std::atoi(argv[1]));

        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}