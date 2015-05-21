#include "tcp_connection.hpp"
#include "tcp_connection_manager.hpp"

#ifdef WITH_TCPNATIVE
//#include <boost/archive/binary_iarchive.hpp>
//#include <boost/archive/binary_oarchive.hpp>

namespace openrc {
namespace tcpnative {

connection::connection(boost::asio::ip::tcp::socket socket, connection_manager& manager, request_handler& handler)
    : socket_(std::move(socket))
    , connection_manager_(manager)
    , request_handler_(handler)
{}

void connection :: start()
{
    //boost::asio::ip::tcp::no_delay option(true);

    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(buffer_, 4),
        [this, self](boost::system::error_code ec, std::size_t bytes_transferred)
        {
            if (!ec)
            {
				uint32_t size;
				memcpy(&size, buffer_.data(), sizeof(uint32_t));
#if 0
                request_parser::result_type result;
                std::tie(result, std::ignore) = request_parser_.parse(
                    request_, buffer_.data(), buffer_.data() + bytes_transferred);

                if (result == request_parser::good)
                {
                    request_handler_.handle_request(request_, reply_);
                    do_write();
                }
                else if (result == request_parser::bad)
                {
                    reply_ = reply::stock_reply(reply::bad_request);
                    do_write();
                }
                else
                {
                    do_read();
                }
#endif
            }
            else if (ec != boost::asio::error::operation_aborted)
            {
                connection_manager_.stop(shared_from_this());
            }
        });

#ifdef WITH_SSL
    //socket_.lowest_layer().set_option(option);
    socket_.async_handshake(boost::asio::ssl::stream_base::server,
            boost::bind(&connection::handle_handshake, shared_from_this(),
              boost::asio::placeholders::error));
#endif
}

#ifdef WITH_SSL
void connection::handle_handshake(const boost::system::error_code& error)
{
    if (!error)
    {
        boost::asio::async_read(socket_, boost::asio::buffer(inbound_header_),
            boost::bind(&connection::handle_read_mainheader, shared_from_this(),
            boost::asio::placeholders::error));
    }
    else
    {
        const std::string text = error.message();
        ErrorStream::write("[Server]: Handshake session: '%s'", text.c_str());
    }
}
#endif

void connection::do_read()
{
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(buffer_, 4),
        [this, self](boost::system::error_code ec, std::size_t bytes_transferred)
        {
            if (!ec)
            {
				uint32_t size;
				memcpy(&size, buffer_.data(), sizeof(uint32_t));
	
				boost::asio::async_read(socket_,
					boost::asio::buffer(buffer_, size),
					[this, self](boost::system::error_code error, std::size_t bytes_transferred)
					{
						// Data.
						// uint32_t size;
						// memcpy(&size, buffer_.data(), sizeof(uint32_t));
					});
            }
            else if (ec != boost::asio::error::operation_aborted)
            {
                connection_manager_.stop(shared_from_this());
            }
        });
}

void connection::do_write()
{
#if 0
    auto self(shared_from_this());
    boost::asio::async_write(socket_, reply_.to_buffers(),
        [this, self](boost::system::error_code ec, std::size_t)
        {
            if (!ec)
            {
                // Initiate graceful connection closure.
                boost::system::error_code ignored_ec;
                socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
            }

            if (ec != boost::asio::error::operation_aborted)
            {
                connection_manager_.stop(shared_from_this());
            }
        });
#endif
}

#if 0
void connection :: handle_read_mainheader(const boost::system::error_code& error)
{
    if(error)
    {       
#ifdef WITH_SSL
        socket_.lowest_layer().close();
#else
        socket_.close();
#endif
        ErrorStream::write(error.message().c_str());
        return;
    }

    MemoryBuffer buffer((BYTE*)inbound_header_, header_length);
    MemoryReader archive(buffer);

    NetHeader header;
    archive >> header;

#ifdef WITH_SSL
    boost::asio::ip::tcp::endpoint remote_ep = socket_.lowest_layer().remote_endpoint();
#else
    boost::asio::ip::tcp::endpoint remote_ep = socket_.remote_endpoint();
#endif
    boost::asio::ip::address remote_ad = remote_ep.address();
    const std::string s = remote_ad.to_string();

    if( header.command != NET_CMD_LOGIN ||
        (header.dataSize != NET_CMD_LOGIN_CLIENT && header.dataSize != NET_CMD_LOGIN_SANDBOX))
    {
        InfoStream::write("[Server]: Bad authentication session: '%s'", s.c_str());
#ifdef WITH_SSL
        socket_.lowest_layer().close();
#else
        socket_.close();
#endif
        return;
    }

    ClientType type = ClientType::None;
    if(header.dataSize == NET_CMD_LOGIN_CLIENT)
        type = ClientType::User;
    else if(header.dataSize == NET_CMD_LOGIN_SANDBOX)
        type = ClientType::Sandbox;

    _listener = _request->createListerner(type, this);

    InfoStream::write("[Server]: Authentication session: '%s'", s.c_str());

    NetHeader outHeader = header;

    outbound_data_.resize(8);
    memcpy(&outbound_data_[0], &outHeader, 8);

    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back(boost::asio::buffer(outbound_data_));

    boost::asio::async_write(socket_, buffers,
        boost::bind(&connection::handle_write, shared_from_this(), boost::asio::placeholders::error)
    );
}

void connection :: handle_read_body(const boost::system::error_code& error, size_t bytes_transferred)
{
    if(error)
    {
        ErrorStream::write(error.message().c_str());
#ifdef WITH_SSL
        socket_.lowest_layer().close();
#else
        socket_.close();
#endif
        return;
    }

    try
    {
        MemoryBuffer buffer((BYTE*)&inbound_data_[0], bytes_transferred);
        MemoryReader archive(buffer);

        NetCommand command;
        archive >> command;

        NetResponse response;
        _request->parse(_listener, command, response);


        scoped_lock<interprocess_mutex> lock(_mutex);

        uint32 fullSize = 0;
        for(const NetCommand& command : response.Commands)
        {
            fullSize += sizeof(uint32) + sizeof(command.command) + command.data.getSize();
        }

        if(fullSize == 0)
        {
            outbound_data_.resize(sizeof(NetHeader));

            NetHeader header;
            header.command = NET_CMD_UPDATE_EMPTY;
            header.dataSize = 0;

            MemoryBuffer buffer_header;
            buffer_header.setOutput(&outbound_data_[0], sizeof(NetHeader));
            MemoryWriter archive_header(buffer_header);
            archive_header << header;

            std::vector<boost::asio::const_buffer> buffers;
            buffers.push_back(boost::asio::buffer(outbound_data_));

            boost::asio::async_write(socket_, buffers,
                boost::bind(&connection::handle_write, shared_from_this(), boost::asio::placeholders::error)
            );

            return;
        }

        outbound_data_.resize(sizeof(NetHeader) + sizeof(uint32) + fullSize);
        std::vector<boost::asio::const_buffer> buffers;

        // Format the header.
        NetHeader header;
        header.command = NET_MESSAGES;
        header.dataSize = fullSize;

        MemoryBuffer buffer;
        buffer.setOutput(&outbound_data_[0], sizeof(NetHeader) + sizeof(uint32) + fullSize);
        MemoryWriter out(buffer);

        out << header;
        out << (uint32)response.Commands.size();

        for(const NetCommand& command : response.Commands)
        {
            uint32 size = sizeof(command.command) + command.data.getSize();
            out << size;
            out << command;
        }

        buffers.push_back(boost::asio::buffer(outbound_data_));

        boost::asio::async_write(socket_, buffers,
            boost::bind(&connection::handle_write, shared_from_this(), boost::asio::placeholders::error)
        );
    }
    catch (std::exception &e)
    {
        ErrorStream::write("%s", e.what());
    }
}

void connection :: handle_read_header(const boost::system::error_code& error)
{
    if(error)
    {
        ErrorStream::write(error.message().c_str());
#ifdef WITH_SSL
        socket_.lowest_layer().close();
#else
        socket_.close();
#endif
        return;
    }

    MemoryBuffer buffer((BYTE*)inbound_header_, header_length);
    MemoryReader archive_header(buffer);

    NetHeader header;
    archive_header >> header;

    uint32 command = header.command;
    uint32 size = header.dataSize;

    switch (command)
    {
    case NET_MESSAGE:
        {
            _listener->onPing();

            inbound_data_.resize(size);
            boost::asio::async_read(socket_, boost::asio::buffer(inbound_data_),
                boost::bind(&connection::handle_read_body, shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred)
            );

            break;
        }
    case NET_CMD_UPDATE:
        {
            _listener->onPing();

            do_write();
            break;
        }
    case NET_CMD_EXIT:
        {
            _request->removeListerner(_listener);
            _listener = nullptr;

#ifdef WITH_SSL
            boost::asio::ip::tcp::endpoint remote_ep = socket_.lowest_layer().remote_endpoint();
#else
            boost::asio::ip::tcp::endpoint remote_ep = socket_.remote_endpoint();
#endif
            boost::asio::ip::address remote_ad = remote_ep.address();
            const std::string s = remote_ad.to_string();

            InfoStream::write("[Server]: Disconnect user '%s'.", s.c_str());
#ifdef WITH_SSL
            socket_.lowest_layer().close();
#else
            socket_.close();
#endif
            break;
        }
    default:
        {
#ifdef WITH_SSL
            boost::asio::ip::tcp::endpoint remote_ep = socket_.lowest_layer().remote_endpoint();
#else
            boost::asio::ip::tcp::endpoint remote_ep = socket_.remote_endpoint();
#endif
            boost::asio::ip::address remote_ad = remote_ep.address();
            const std::string s = remote_ad.to_string();

            ErrorStream::write("[Server]: Bad command. Session '%s' close.", s.c_str());
#ifdef WITH_SSL
            socket_.lowest_layer().close();
#else
            socket_.close();
#endif
        }
    }
}

void connection :: handle_write(const boost::system::error_code& error)
{
    if (!error)
    {
        do_read();
    }
}
#endif

} // namespace tcpnative
} // namespace openrc.

#endif