//
// sslserver.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "sslserver.hpp"
#include <signal.h>
#include <utility>

#ifdef WITH_SSL

namespace http {
namespace server {

ssl_server::ssl_server(const std::string& address, const std::string& port,
    const std::string& doc_root)
    : io_service_()
    , signals_(io_service_)
    , acceptor_(io_service_)
    , connection_manager_()
    , socket_(io_service_)
    , request_handler_(doc_root)
    , context_(boost::asio::ssl::context::sslv23)
{
    // Register to handle the signals that indicate when the server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through Asio.
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
#if defined(SIGQUIT)
    signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)

    do_await_stop();

    context_.set_options(
            boost::asio::ssl::context::default_workarounds
            | boost::asio::ssl::context::no_sslv2
            | boost::asio::ssl::context::single_dh_use);
    //context_.set_password_callback(boost::bind(&TcpServer::get_password, this));
    context_.use_certificate_chain_file("server.crt");
    context_.use_private_key_file("server.key", boost::asio::ssl::context::pem);
    context_.use_tmp_dh_file("dh512.pem");

    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    boost::asio::ip::tcp::resolver resolver(io_service_);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({address, port});
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    do_accept();
}

void ssl_server::run()
{
    // The io_service::run() call will block until all asynchronous operations
    // have finished. While the server is running, there is always at least one
    // asynchronous operation outstanding: the asynchronous accept call waiting
    // for new incoming connections.
    io_service_.run();
}

void ssl_server::do_accept()
{
    acceptor_.async_accept(socket_,
        [this](boost::system::error_code ec)
        {
            // Check whether the server was stopped by a signal before this
            // completion handler had a chance to run.
            if (!acceptor_.is_open())
            {
                return;
            }

            if (!ec)
            {
                connection_manager_.start(std::make_shared<connection>(
                std::move(socket_), connection_manager_, request_handler_, context_));
            }

            do_accept();
        });
}

void ssl_server::do_await_stop()
{
    signals_.async_wait(
        [this](boost::system::error_code /*ec*/, int /*signo*/)
        {
            // The server is stopped by cancelling all outstanding asynchronous
            // operations. Once all operations have finished the io_service::run()
            // call will exit.
            acceptor_.close();
            connection_manager_.stop_all();
        });
}

} // namespace server
} // namespace http

#endif
