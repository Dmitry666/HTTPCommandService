#include "web_connection.hpp"
#include "web_connection_manager.hpp"
#include "../crypt.h"

#ifdef WITH_WEBSOCKET

//#include "webserver.h"
//#include "crypt.h"
//#include "codes.h"

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>

//#include <boost/bind.hpp>

#include <sstream>
#include <string>

namespace openrc {
namespace websocket {

template<typename T>
class HeaderParser
{
    /**
    GET /chat HTTP/1.1
    Host: server.example.com
    Upgrade: websocket
    Connection: Upgrade
    Origin: http://javascript.ru
    Sec-WebSocket-Key: Iv8io/9s+lYFgZWcXczP8Q==
    Sec-WebSocket-Version: 13
    */
    typedef std::map<std::string, std::string> NameToValue;

public:
    static typename T::const_iterator readSpaces(typename T::const_iterator it, typename T::const_iterator end)
    {
        while(it != end && *it == ' ')
            ++it;
        return it;
    }

    static std::tuple<bool, typename T::const_iterator, std::string> readLine(typename T::const_iterator it, typename T::const_iterator end)
    {
        typename T::const_iterator start = it;
        std::string text;

        while(it != end)
        {
            if(*it == '\n')
                return std::tuple<bool, typename T::const_iterator, std::string>(true, it + 1, text);

            text.push_back(*it);
            ++it;
        }

        return std::tuple<bool, typename T::const_iterator, std::string>(false, end, "");
    }

    static std::tuple<bool, typename T::const_iterator, std::string, std::string> readField(typename T::const_iterator it, typename T::const_iterator end)
    {
        typename T::const_iterator start = it;
        std::string name, value;

        while(it != end)
        {
            if(*it == ':')
                break;

            name.push_back(*it);
            ++it;
        }

        if(it == end)
            return std::tuple<bool, typename T::const_iterator, std::string, std::string>(false, end, "", "");

        ++it;
        it = readSpaces(it, end);

        while(it != end)
        {
            if(*it == '\n')
                return std::tuple<bool, typename T::const_iterator, std::string, std::string>(true, it + 1, name, value);

            if(*it != '\r')
                value.push_back(*it);

            ++it;
        }

        return std::tuple<bool, typename T::const_iterator, std::string, std::string>(false, end, "", "");
    }

    bool parse(typename T::const_iterator it, typename T::const_iterator end)
    {
        std::tuple<bool, std::string> result;

        while(it != end)
        {
            switch (*it)
            {
            case 'G': // GET /chat HTTP/1.1
                {
                    std::tuple<bool, typename T::const_iterator, std::string> getLine = readLine(it, end);
                    if(!std::get<0>(getLine))
                        return false;

                    it = std::get<1>(getLine);
                    break;
                }
            case '\r':
                {
                    std::get<0>(result) = true;
                    return true;
                }
            default:
                {
                    std::tuple<bool, typename T::const_iterator, std::string, std::string> field = readField(it, end);
                    if(!std::get<0>(field))
                        return false;

                    _fields.insert(std::make_pair(std::get<2>(field), std::get<3>(field)));

                    it = std::get<1>(field);

                    break;
                }
            }
        }

        return false;
    }

    std::string  operator [] (const std::string& id)
    {
        NameToValue::iterator it = _fields.find(id);
        if(it == _fields.end())
            return "";

        return it->second;
    }

protected:
    NameToValue _fields;
};

class Convert
{
public:
    static std::string toBase64(std::string text)
    {
        using namespace boost::archive::iterators;

        unsigned int paddedCharacters = 0;
        while(text.size() % 3 != 0)
        {
            paddedCharacters++;
            text.push_back(0x00);
        }

        /*
        typedef
            insert_linebreaks<      // insert line breaks every 72 characters
            base64_from_binary<		// convert binary values ot base64 characters
            transform_width<		// retrieve 6 bit integers from a sequence of 8 bit bytes
            const char *,
            6,
            8
            >
            >
            ,72
            >
            base64Iterator; // compose all the above operations in to a new iterator
        */
        // Crazy typedef black magic
        typedef
            insert_linebreaks<         // insert line breaks every 76 characters
            base64_from_binary<    // convert binary values to base64 characters
            transform_width<   // retrieve 6 bit integers from a sequence of 8 bit bytes
            const unsigned char *
            ,6
            ,8
            >
            >
            ,76
            >
            base64Iterator; // compose all the above operations in to a new iterator

        /*
        std::stringstream os;
        std::copy(
            base64Iterator(text.c_str()),
            base64Iterator(text.c_str() + text.size()),
            ostream_iterator<char>(os)
            );
        */

        std::string encodedString(
            base64Iterator(text.c_str()),
            base64Iterator(text.c_str() + (text.size() - paddedCharacters)));

        for(unsigned int i = 0; i < paddedCharacters; i++)
            encodedString.push_back('=');

        return encodedString;
    }
};

class Acepter
{
public:

    static std::string acceptKey(const std::string& key)
    {
        static std::string guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        std::string longKey = key + guid;

        std::string hash = crypt::SHA1Crypt::computeHash(longKey);
        std::string acceptKey = Convert::toBase64(hash);

        return acceptKey;
    }
};

/**
    Answer.

    HTTP/1.1 101 Switching Protocols
    Upgrade: websocket
    Connection: Upgrade
    Sec-WebSocket-Accept: hsBlbuDTkk24srzEOTBUlZAlC2g=


    Frame.

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    +-+-+-+-+-------+-+-------------+-------------------------------+
    |F|R|R|R| опкод |М| Длина тела  |    � асширенная длина тела     |
    |I|S|S|S|(4бита)|А|   (7бит)    |            (1 байт)           |
    |N|V|V|V|       |С|             |(если длина тела==126 или 127) |
    | |1|2|3|       |К|             |                               |
    | | | | |       |А|             |                               |
    +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
    |  Продолжение расширенной длины тела, если длина тела = 127    |
    + - - - - - - - - - - - - - - - +-------------------------------+
    |                               |  Ключ маски, если МАСКА = 1   |
    +-------------------------------+-------------------------------+
    | Ключ маски (продолжение)      |       Данные фрейма ("тело")  |
    +-------------------------------- - - - - - - - - - - - - - - - +
    :                     Данные продолжаются ...                   :
    + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
    |                     Данные продолжаются ...                   |
    +---------------------------------------------------------------+
*/

class commit_at_least
{
public:
    explicit commit_at_least(std::size_t size)
        : size (size)
        , total(0)
    {}

    template <typename Iterator>
    std::pair<Iterator, bool> operator() (Iterator begin, Iterator end)
    {
        std::size_t previous = total;
        total += std::distance(begin, end);
        if(total < size)
        {
            return std::make_pair(end, false);
        }
        else
        {
            std::advance(begin, size - previous);
            return std::make_pair(begin, true);
        }
    }

private:
    std::size_t size;
    std::size_t total;
};

/*
namespace boost
{
    namespace asio
    {
        template <> struct is_match_condition<commit_at_least> : public boost::true_type
        {};
    }
}
*/

/**
    Server.
*/
connection::connection(boost::asio::ip::tcp::socket socket, connection_manager& manager, request_handler& handler)
    : socket_(std::move(socket))
    , connection_manager_(manager)
    , request_handler_(handler)
{}

void connection :: start()
{
    //boost::asio::ip::tcp::no_delay option(true);
    //socket_.set_option(option);

    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(buffer_),
        [this, self](boost::system::error_code ec, std::size_t bytes_transferred)
        {
            if (!ec)
            {
				HeaderParser<std::array<char, 8192>> parser;
				bool result = parser.parse( buffer_.cbegin(), buffer_.cbegin() + bytes_transferred);
				if (result)
				{
					boost::asio::streambuf request;
					std::ostream request_stream(&request);
					request_stream << "HTTP/1.1 101 Switching Protocols\r\n";
					request_stream << "Upgrade: websocket\r\n";
					request_stream << "Connection: Upgrade\r\n";
					request_stream << "Sec-WebSocket-Accept: " << Acepter::acceptKey(parser["Sec-WebSocket-Key"]) << "\r\n\r\n";

					memcpy(buffer_.data(), boost::asio::buffer_cast<const char*>( request.data()), request.size()); 

					boost::asio::async_write(socket_,
						boost::asio::buffer(buffer_, request.size()),
						[this, self](boost::system::error_code ec, std::size_t)
						{
							if (!ec)
							{
								do_read();

								//boost::system::error_code ignored_ec;
								//socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
							}

							if (ec != boost::asio::error::operation_aborted)
							{
								connection_manager_.stop(self);
							}							
						});
				}
				else
				{
					connection_manager_.stop(self);
				}
            }
            else if (ec != boost::asio::error::operation_aborted)
            {
                connection_manager_.stop(self);
            }
        });
}

void connection::stop()
{}

void connection::do_read()
{
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(buffer_, 2),
        [this, self](boost::system::error_code ec, std::size_t bytes_transferred)
        {
            if (!ec)
            {
				uint16_t command;
				memcpy(&command, buffer_.data(), sizeof(uint16_t));
				WebHeader header = parse_header(command);

				if( header.leght == 126) // Need read size 2 bytes
				{
					boost::asio::async_read(socket_,
						boost::asio::buffer(buffer_, 2),
						[this, self, header](boost::system::error_code error, std::size_t bytes_transferred)
						{
							if (error)
							{
								connection_manager_.stop(self);
								return;
							}

							uint8_t* in_buffer = (uint8_t*)buffer_.data();
							uint16_t size = in_buffer[1] + (in_buffer[0]<<8);

							boost::asio::async_read(socket_,
								boost::asio::buffer(buffer_, size + header.mask * 4),
								[this, self, header](boost::system::error_code error, std::size_t bytes_transferred)
								{
									handle_read_data(header, error, bytes_transferred);
								});
						});
				}
				else if(header.leght == 127) // Need read size 4 bytes
				{
					boost::asio::async_read(socket_,
						boost::asio::buffer(buffer_, 4),
						[this, self, header](boost::system::error_code error, std::size_t bytes_transferred)
						{
							if (error)
							{
								connection_manager_.stop(self);
								return;
							}

							uint32_t size;
							memcpy(&size, buffer_.data(), sizeof(uint32_t));

							boost::asio::async_read(socket_,
								boost::asio::buffer(buffer_, size + header.mask * 4),
								[this, self, header](boost::system::error_code error, std::size_t bytes_transferred)
								{
									handle_read_data(header, error, bytes_transferred);
								});
						}
					);
				}
				else if(header.leght) // Read data.
				{
					boost::asio::async_read(socket_,
						boost::asio::buffer(buffer_, header.leght + header.mask * 4),
						[this, self, header](boost::system::error_code error, std::size_t bytes_transferred)
						{
							handle_read_data(header, error, bytes_transferred);
						});
				}
                else
                {
					connection_manager_.stop(self);
                }
            }
            else if (ec != boost::asio::error::operation_aborted)
            {
                connection_manager_.stop(self);
            }
        });
}

void connection::do_write()
{
	std::vector<boost::asio::const_buffer> buffers;
	std::vector<boost::asio::const_buffer> body_buffers = reply_.to_buffers();

    uint32_t nb_bytes = 0;
	for(auto& buf : body_buffers)
		nb_bytes += boost::asio::buffer_size(buf);


	if(nb_bytes < 126)
	{
		header_.header = make_header(1, 0x1, 0, nb_bytes);
        nb_bytes += sizeof(uint16_t);

		buffers.push_back(boost::asio::buffer(&header_.header, sizeof(uint16_t)));
	}
	else
	{
        uint16_t nb_bytes_16 = nb_bytes;
		header_.header = make_header(1, 0x1, 0, 126);

		buffers.push_back(boost::asio::buffer(&header_.header, sizeof(uint16_t)));

		uint8_t* b = reinterpret_cast<uint8_t*>(&nb_bytes_16);
			
		header_.ext[0] = b[1];
		header_.ext[1] = b[0];

		buffers.push_back(boost::asio::buffer(&header_.ext, sizeof(uint16_t)));
	}

	buffers.insert(buffers.end(), body_buffers.begin(), body_buffers.end());
	//
	
    auto self(shared_from_this());
    boost::asio::async_write(socket_, buffers,
        [this, self](boost::system::error_code ec, std::size_t)
        {
            if (!ec)
            {
				do_read();
                //boost::system::error_code ignored_ec;
                //socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
            }

            if (ec != boost::asio::error::operation_aborted)
            {
                connection_manager_.stop(self);
            }	
        });
}

void connection :: handle_read_data(WebHeader header, const boost::system::error_code& e, std::size_t bytes_transferred)
{
    uint32_t offset = 0;

    uint32_t bytes = bytes_transferred;
    if(header.mask)
    {
        bytes -= 4;

        uint32_t mask = *reinterpret_cast<uint32_t*>(buffer_.data());
        unsigned char* c = (unsigned char*)buffer_.data() + 4;
        for(int i=0; i<bytes; i++)
            c[i] = c[i] ^ ((unsigned char*)(&mask))[i%4];

        offset += 4;
    }


    //char* data = buffer_.data() + offset;

#if 0
    if( header.opcode == 9 ) // this is ping
    {
        // Send pong.
        int16_t header = make_header(1, 0xA, 0, bytes);

        memcpy(_sendData.data(), &header, sizeof(uint16));
        memcpy(_sendData.data() + sizeof(uint16), data, bytes);

        boost::asio::async_write(socket_,
            boost::asio::buffer(_sendData, bytes + sizeof(uint16)),
            make_custom_alloc_handler(allocator_,
            boost::bind(&connection::handle_write,shared_from_this(),boost::asio::placeholders::error))
            );

        return;
    }

    // Parse data block.
    if(bytes < 8)
    {
        return;
    }
#endif

    uint32_t offsetData = 0;

	int16_t size = header.leght;
		//*reinterpret_cast<int16_t*>(data + offsetData);
    //offsetData += sizeof(int16_t);

    request_parser::result_type result;
    std::tie(result, std::ignore) = request_parser_.parse(
        request_, buffer_.data() + offset, buffer_.data() + offset + bytes_transferred);

	reply_.reset();
    if (result == request_parser::good)
    {
		if(request_.command == "command")
		{
			request_handler_.handle_request(request_, reply_);
		}
		else if(request_.command == "update")
		{}
		else
		{
			reply_ = reply::stock_reply(reply::bad_request);
		}

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
}

} // namespace websocket
} // namespace openrc.


#endif