//
// connection.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTPS_CONNECTION_HPP
#define HTTPS_CONNECTION_HPP

#ifdef WITH_SSL

#include <array>
#include <memory>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "reply.hpp"
#include "request.hpp"
#include "request_handler.hpp"
#include "request_parser.hpp"

namespace http {
namespace server {

class connection_manager;

/// Represents a single connection from a client.
class ssl_connection
    : public std::enable_shared_from_this<ssl_connection>
{
public:
    ssl_connection(const ssl_connection&) = delete;
    ssl_connection& operator=(const ssl_connection&) = delete;

    /// Construct a connection with the given socket.
    explicit ssl_connection(
            boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket,
            connection_manager& manager,
            request_handler& handler,
            boost::asio::ssl::context& context);

    /// Start the first asynchronous operation for the connection.
    void start();

    /// Stop all asynchronous operations associated with the connection.
    void stop();

private:
    ///
    void handle_handshake(const boost::system::error_code& error);

    /// Perform an asynchronous read operation.
    void do_read();

    /// Perform an asynchronous write operation.
    void do_write();

    /// Ssl socket for the connection.
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;

    /// The manager for this connection.
    connection_manager& connection_manager_;

    /// The handler used to process the incoming request.
    request_handler& request_handler_;

    /// Buffer for incoming data.
    std::array<char, 8192> buffer_;

    /// The incoming request.
    request request_;

    /// The parser for the incoming request.
    request_parser request_parser_;

    /// The reply to be sent back to the client.
    reply reply_;
};

typedef std::shared_ptr<connection> connection_ptr;

} // namespace server
} // namespace http

#endif

#endif // HTTP_CONNECTION_HPP
