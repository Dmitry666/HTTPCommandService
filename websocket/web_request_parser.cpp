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

#include "json/json.h"
#include "json/reader.h"

#include "../common-private.h"

using namespace std;

namespace openrc {
namespace websocket {

request_parser::request_parser()
{}

void request_parser::reset()
{
    //state_ = method_start;
}

request_parser::result_type request_parser::parse_json(request& req, const std::string& text)
{
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(text, root);
	if ( !parsingSuccessful )
	{
		// report to the user the failure and their locations in the document.
		//std::cout  << "Failed to parse configuration\n"
		//		   << reader.getFormattedErrorMessages();
		return bad;
	}

	const Json::Value commandValue = root["code"];
	const Json::Value bodyValue = root["body"];

	req.command = commandValue.asString();
	req.body = bodyValue.asString();

	// And you can write to a stream, using the StyledWriter automatically.
#ifdef _DEBUG
	std::cout << root;
#endif

	return good;
}

} // namespace websocket
} // namespace openrc.