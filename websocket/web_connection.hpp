// Author: Oznabikhin Dmitry
// Email: gamexgroup@gmail.ru
//
// Copyright (c) GameX. All rights reserved.

#ifndef WEBSOCKET_CONNECTION_HPP
#define WEBSOCKET_CONNECTION_HPP

#ifdef WITH_WEBSOCKET


#include "../common-private.h"
#include "web_request_handler.hpp"

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
namespace websocket {

class connection_manager;

/**
    Thread session from client.
*/
class connection
    : public std::enable_shared_from_this<connection>
{
    struct WebHeader
    {
        BYTE fin;
        BYTE rsv1;
        BYTE rsv2;
        BYTE rsv3;

        BYTE opcode;
        BYTE mask;
        BYTE leght;

        BYTE empty;
        //SHORT size;
    };

public:
	connection(const connection&) = delete;
    connection& operator=(const connection&) = delete;

    connection(boost::asio::ip::tcp::socket socket, connection_manager& manager, request_handler& handler);

    /// Start the first asynchronous operation for the connection.
    void start();

    /// Stop all asynchronous operations associated with the connection.
    void stop();

private:
    /// Perform an asynchronous read operation.
    void do_read();

    /// Perform an asynchronous write operation.
    void do_write();

private:
    /// Socket for the connection.
    boost::asio::ip::tcp::socket socket_;

    /// The manager for this connection.
    connection_manager& connection_manager_;

    /// The handler used to process the incoming request.
    request_handler& request_handler_;

    /// Buffer for incoming data.
    std::array<char, 8192> buffer_;

#if 0
private:
    void handle_read_mainheader(const boost::system::error_code& e, std::size_t bytes_transferred);
    void handle_read_data(WebHeader header, const boost::system::error_code& e, std::size_t bytes_transferred);

    //void handle_read_buffer(const boost::system::error_code& error,size_t bytes_transferred);
    //void handle_read_function(const boost::system::error_code& error,size_t bytes_transferred);

    void handle_read_header(const boost::system::error_code& error);
    //void handle_read_extendsize(const boost::system::error_code& error);

    void handle_write(const boost::system::error_code& error);
#endif
    //
    static USHORT make_header(BYTE fin, BYTE opcode, BYTE mask, BYTE leght)
    {
        uint16_t value = 0;

        BYTE* out_buffer = reinterpret_cast<BYTE*>(&value);

        out_buffer[0] = (opcode & 0x0F) + (fin << 7);
        out_buffer[1] = (leght & (~0x80)) + (mask << 7);

        return value;
    }

    // Read header.
    static BYTE read_bit(uint16_t value, uint32_t k)
    {
        return (value & (1 << k)) >> k;
    }

    static BYTE read_halfbyte(uint16_t value, uint32_t k)
    {
        return (value & (15 << k)) >> k;
    }

    static BYTE read_byte(uint16_t value, uint32_t k)
    {
        return (value & (255 << k)) >> k;
    }

    static uint16_t read_short(uint16_t value, uint32_t k)
    {
        return (value & (64535 << k)) >> k;
    }

    static WebHeader parse_header(uint16_t value)
    {
        WebHeader header;
        BYTE* in_buffer = reinterpret_cast<BYTE*>(&value);

        header.opcode = in_buffer[0] & 0x0F;
        header.fin = (in_buffer[0] >> 7) & 0x01;
        header.mask = (in_buffer[1] >> 7) & 0x01;
        header.leght = in_buffer[1] & (~0x80);

        return header;
    }
#if 0

private:
    // Buffer used to store data received from the client.
    boost::array<BYTE, 65536> _inData;					// Input data from client.
    boost::array<BYTE, 65536> _sendData;				// Input data from client.

    std::vector<boost::asio::const_buffer> send_buffer;	// Send buffer.

    boost::array<BYTE, 8> _headerData;					// Input data from client.

    handler_allocator allocator_;
#endif
};

typedef std::shared_ptr<connection> connection_ptr;

} // namespace websocket
} // namespace openrc.

#endif
#endif