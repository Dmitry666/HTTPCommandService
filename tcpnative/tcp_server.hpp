// Author: Oznabikhin Dmitry
// Email: gamexgroup@gmail.ru
//
// Copyright (c) GameX. All rights reserved.
#pragma once

#ifdef WITH_TCPNATIVE

#include "tcp_connection.hpp"
#include "tcp_connection_manager.hpp"
#include "tcp_request_handler.hpp"

#include "../server.h"

namespace openrc {
namespace tcpnative {

/**
    Main net server class.
*/
class tcp_server : public base_server
{
public:
    tcp_server(const tcp_server&) = delete;
    tcp_server& operator=(const tcp_server&) = delete;

    /// Construct the server to listen on the specified TCP address and port, and
    /// serve up files from the given directory.
    explicit tcp_server(const std::string& address, const std::string& port, const std::string& doc_root);

    /// Run the server's io_service loop.
    virtual void run() override;

	virtual void stop() override;

private:
    /// Perform an asynchronous accept operation.
    void do_accept();

    /// Wait for a request to stop the server.
    void do_await_stop();

private:
    /// The io_service used to perform asynchronous operations.
    boost::asio::io_service io_service_;

#ifdef WITH_SSL
    boost::asio::ssl::context context_;
#endif

    /// The signal_set is used to register for process termination notifications.
    boost::asio::signal_set signals_;

    /// Acceptor used to listen for incoming connections.
    boost::asio::ip::tcp::acceptor acceptor_;

    /// The connection manager which owns all live connections.
    connection_manager connection_manager_;

    /// The next socket to be accepted.
    boost::asio::ip::tcp::socket socket_;

    /// The handler for all incoming requests.
    request_handler request_handler_;
};

} // namespace tcpnative
} // namespace openrc.

#endif