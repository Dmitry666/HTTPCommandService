// Author: Oznabikhin Dmitry
// Email: gamexgroup@gmail.ru
//
// Copyright (c) UTS. All rights reserved.
#pragma once

#ifdef WITH_TCPNATIVE

#include "../common-private.h"
#include "tcp_request_handler.hpp"
#include "tcp_reply.hpp"
#include "tcp_request_parser.hpp"
#include "tcp_request.hpp"

#include <array>
#include <memory>

#ifndef _MSC_VER
// save diagnostic state
#pragma GCC diagnostic push
// turn off the specific warning. Can also use "-Wall"
#pragma GCC diagnostic ignored "-Wall"
//#pragma GCC diagnostic ignored "-Wunused-variable"
//#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#define BOOST_CONFIG_WARNING_DISABLE_HPP
#pragma GCC system_header
#endif

#include <boost/asio.hpp>

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif


namespace openrc {
namespace tcpnative {

#if 0
/**
 * @brief Main header in net package.
 */
struct NetHeader
{
    uint32 command;
    uint32 dataSize;

    /**
     * @brief Constructor.
     */
    NetHeader()
        : command(0)
        , dataSize(0)
    {}

    /**
     * @brief Copy constructor.
     * @param header
     */
    NetHeader(const NetHeader& header)
        : command(header.command)
        , dataSize(header.dataSize)
    {}

    /**
     * @brief operator =
     * @param header
     * @return
     */
    NetHeader& operator = (const NetHeader& header)
    {
        command = header.command;
        dataSize = header.dataSize;

        return (*this);
    }

    /**
     * @brief Deserialize operator.
     * @param buffer archive buffer.
     * @param header data object.
     * @return archive buffer.
     */
    friend SBuffer& operator >> (SBuffer& buffer, NetHeader& header)
    {
        buffer >> header.command;
        buffer >> header.dataSize;

        return buffer;
    }

    /**
     * @brief Serialize operator.
     * @param buffer archive buffer.
     * @param header data object.
     * @return archive buffer.
     */
    friend SBuffer& operator << (SBuffer& buffer, const NetHeader& header)
    {
        buffer << header.command;
        buffer << header.dataSize;

        return buffer;
    }
};
#endif

class connection_manager;

/**
 * @brief Thread session from client.
 */
class connection
    : public std::enable_shared_from_this<connection>
{
#ifdef WITH_SSL
    typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;
#endif

public:
    connection(const connection&) = delete;
    connection& operator=(const connection&) = delete;

    /// Construct a connection with the given socket.
    explicit connection(boost::asio::ip::tcp::socket socket,
      connection_manager& manager, request_handler& handler);

#if 0
#ifdef WITH_SSL
    ssl_socket::lowest_layer_type& socket()
    {
        return socket_.lowest_layer();
    }
#else
    tcp::socket& socket()
    {
        return socket_;
    }
#endif
#endif

#ifdef WITH_SSL
    void handle_handshake(const boost::system::error_code& error);
#endif


    /// Start the first asynchronous operation for the connection.
    void start();

    /// Stop all asynchronous operations associated with the connection.
    void stop();

private:
    /**
     * @brief Async read session.
     */
    void do_read();

    /**
     * @brief Push all async commands and write.
     */
    void do_write();

#if 0
    /**
     * @brief On read main header.
     * @param e error.
     */
    void handle_read_mainheader(const boost::system::error_code& e);

    /**
     * @brief On read body.
     * @param error error code.
     * @param bytes_transferred bytes transferred.
     */
    void handle_read_body(const boost::system::error_code& error,size_t bytes_transferred);

    /**
     * @brief On read header.
     * @param error error code.
     */
    void handle_read_header(const boost::system::error_code& error);

    /**
     * @brief On write.
     * @param error error code.
     */
    void handle_write(const boost::system::error_code& error);

    /// Asynchronously write a data structure to the socket.
    template <typename T, typename Handler>
    void async_write(const T& t, Handler handler)
    {
        // Serialize the data first so we know how large it is.
        MemoryBuffer buffer_data;
        MemoryWriter archive_data(buffer_data);
        archive_data << t;

        outbound_data_.resize(buffer_data.getSize());
        memcpy(&outbound_data_[0], buffer_data.getData(), buffer_data.getSize());

        // Format the header.
        NetHeader header;
        header.command = NET_MESSAGE;
        header.dataSize = buffer_data.getSize();

        MemoryBuffer buffer_header;
        MemoryWriter archive_header(buffer_header);
        archive_header << header;

        outbound_header_.resize(buffer_header.getSize());
        memcpy(&outbound_header_[0], buffer_header.getData(), buffer_header.getSize());

        // Write the serialized data to the socket. We use "gather-write" to send
        // both the header and the data in a single write operation.
        std::vector<boost::asio::const_buffer> buffers;
        buffers.push_back(boost::asio::buffer(outbound_header_));
        buffers.push_back(boost::asio::buffer(outbound_data_));
        boost::asio::async_write(socket_, buffers, handler);
    }
#endif

private:
    //! The socket used to communicate with the client.
#ifdef WITH_SSL
    ssl_socket socket_;
#else
    boost::asio::ip::tcp::socket socket_;
#endif

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

#if 0
	////!!!!!
    //! The size of a fixed length header.
    enum { header_length = 8 };

    //! Holds an inbound header.
    char inbound_header_[header_length];

    //! Holds an outbound header.
    std::string outbound_header_;

    //! Holds the outbound data.
    std::string outbound_data_;

    //! Holds the inbound data.
    std::vector<char> inbound_data_;
#endif
};

typedef std::shared_ptr<connection> connection_ptr;

} // namespace tcpnative
} // namespace openrc.

#endif