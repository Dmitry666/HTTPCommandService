//
// request_parser.cpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "web_request_parser.hpp"
#include "web_request.hpp"

namespace openrc {
namespace websocket {

request_parser::request_parser()
    : state_(method_start)
{}

void request_parser::reset()
{
    state_ = method_start;
}

request_parser::result_type request_parser::parse_json(const std::string& text)
{
	return bad;
}

} // namespace websocket
} // namespace openrc.