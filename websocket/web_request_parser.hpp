#ifndef WEB_REQUEST_PARSER_HPP
#define WEB_REQUEST_PARSER_HPP

#include <tuple>
#include <string>

namespace openrc {
namespace websocket {

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
			std::string text(begin, end);
            result_type result = parse_json(req, text);
            if (result == good || result == bad)
                return std::make_tuple(result, begin);
        }

        return std::make_tuple(indeterminate, begin);
    }

private:
	result_type parse_json(request& req, const std::string& text);
};

} // namespace websocket
} // namespace openrc.

#endif // WEB_REQUEST_PARSER_HPP
