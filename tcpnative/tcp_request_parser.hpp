#ifndef TCP_REQUEST_PARSER_HPP
#define TCP_REQUEST_PARSER_HPP

#include <tuple>
#include <string>

namespace openrc {
namespace tcpnative {

struct request;

/// Parser for incoming requests.
class request_parser
{
public:
    /// Construct ready to parse the request method.
    request_parser();

    /// Reset to initial parser state.
    void reset();

    /// Result of parse.
    enum result_type { good, bad, indeterminate };

    template <typename InputIterator>
    std::tuple<result_type, InputIterator> parse(request& req, InputIterator begin, InputIterator end)
    {
        while (begin != end)
        {
            result_type result = consume(req, *begin++);
            if (result == good || result == bad)
                return std::make_tuple(result, begin);
        }
        return std::make_tuple(indeterminate, begin);
    }

private:
	result_type parse_json(const std::string& text);
};

} // namespace tcpnative
} // namespace openrc.

#endif // TCP_REQUEST_PARSER_HPP
