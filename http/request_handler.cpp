//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "request_handler.hpp"
#include <fstream>
#include <sstream>
#include <string>

#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"

#include "../controller.h"
#include "../sessionmanager.h"

using namespace std;

namespace openrc {
namespace http {
namespace server {

vector<string> split(const string& str, char delimiter)
{
    vector<string> internal;
    stringstream ss(str); // Turn the string into a stream.
    string tok;

    while(getline(ss, tok, delimiter)) {
        internal.push_back(tok);
    }

    while(!internal.empty() && internal.front().empty())
        internal.erase(internal.begin());

    return internal;
}

request_handler::request_handler(const string& doc_root)
    : doc_root_(doc_root)
{}

void request_handler::handle_request(const request& req, reply& rep)
{
    SessionKey sessionKey;
    for(const header& h : req.headers)
    {

        if(h.name == "Cookie")
        {
            const vector<string> blocks = split(h.value, ';');
            for(const string& block : blocks)
            {
                const vector<string> pair = split(block, '=');
                if(pair.size() == 2 && pair[0] == "RMID")
                {
                    sessionKey = pair[1];
                }
            }
        }
        else if(h.name == "RMID")
        {
            sessionKey = h.value;
        }
    }
    // Decode url to path.
    //req.headers
    string request_path;
    if (!url_decode(req.uri, request_path))
    {
        rep = reply::stock_reply(reply::bad_request);
        return;
    }

    // Request path must be absolute and not contain "..".
    if (request_path.empty() || request_path[0] != '/'
        || request_path.find("..") != string::npos)
    {
        rep = reply::stock_reply(reply::bad_request);
        return;
    }

    // If path ends in slash (i.e. is a directory) then add "index.html".
    if (request_path[request_path.size() - 1] == '/')
    {
        request_path += "index.html";
		handle_request_page(request_path, req, rep);
		return;
    }

	//
	auto blocks = split(request_path, '?');

	if(blocks.empty())
	{
		rep = reply::stock_reply(reply::not_found);
		return;
	}

	const string& blockA = blocks[0];
	const string blockB = blocks.size() > 1 ? blocks[1] : "";

	auto controllerBlocks = split(blockA, '/');

	if(controllerBlocks.size() != 2)
	{
		handle_request_page(request_path, req, rep);
		return;
	}

	const string controllerName = controllerBlocks[0];
	const string commandName = controllerBlocks[1];

	const vector<string> arguments = split(blockB, '&');

	IController* icontroller = ControllerManager::FindController(controllerName);
	if(icontroller == nullptr)
	{
		handle_request_page(request_path, req, rep);
		return;
	}

	if( !icontroller->BeginAction() )
	{
		rep = reply::stock_reply(reply::not_found);
		return;
	}

	ControllerMethodRef methodRef = icontroller->FindMethod(commandName);
	if( methodRef == nullptr )
	{
		rep = reply::stock_reply(reply::not_found);
		return;
	}

	map<string, string> argumentsMap;
	for(auto& argument : arguments)
	{
		const vector<string> ts = split(argument, '=');
		if(ts.size() == 2)
		{
			argumentsMap.insert(std::make_pair(ts[0], ts[1]));
		}
	}

#ifdef WITH_COOKIE
	SessionWeak sessionWeak = sessionKey.empty() ?
				SessionManager::NewSession() :
				SessionManager::FindSessionByKey(sessionKey);

	if(sessionWeak.expired())
	{
		sessionWeak = SessionManager::NewSession();
	}
#else
	SessionId sessionId;
#endif

	SessionShared session = sessionWeak.lock();

	bool validate = methodRef->IsValidateMethod() ? methodRef->Validate(icontroller, sessionWeak, argumentsMap) :
			icontroller->Validate(sessionWeak, argumentsMap);

	if(!validate)
	{
		rep = reply::stock_reply(reply::not_found);
		return;
	}

	ControllerOutput output;

    try
    {
        (*methodRef)(icontroller, sessionWeak, argumentsMap, output);
    }
    catch(std::exception e)
    {
        std::cout << "Call controller methods exception: " << e.what() << std::endl;
    }

	rep.content = output.GetBody();
	icontroller->EndAction();

	rep.status = reply::ok;
#ifdef WITH_COOKIE
    rep.headers.resize(4 + 3 * int32(sessionKey.empty() || sessionKey != session->Id.Key));
#else
    rep.headers.resize(4);
#endif

    rep.headers[0].name = "Content-Length";
    rep.headers[0].value = std::to_string(rep.content.size());
    rep.headers[1].name = "Content-Type";
    //rep.headers[1].value = mime_types::extension_to_type(extension);

	rep.headers[2].name = "Access-Control-Allow-Origin";
    rep.headers[2].value = "*";

	rep.headers[3].name = "Access-Control-Allow-Headers";
    rep.headers[3].value = "Origin, X-Requested-With, Content-Type, Accept";

#ifdef WITH_COOKIE
    if(sessionKey.empty() || sessionKey != session->Id.Key)
    {
        rep.headers[4].name = "Set-Cookie";
        rep.headers[4].value = "RMID=" + session->Id.Key + ";";

        rep.headers[5].name = "Access-Control-Expose-Headers";
        rep.headers[5].value = "Set-Cookie";

        rep.headers[6].name = "RMID";
        rep.headers[6].value = session->Id.Key;
    }
#endif
}

void request_handler::handle_request_page(const std::string& request_path, const request& req, reply& rep)
{
	// Determine the file extension.
	std::size_t last_slash_pos = request_path.find_last_of("/");
	std::size_t last_dot_pos = request_path.find_last_of(".");
	std::string extension;
	if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
	{
		extension = request_path.substr(last_dot_pos + 1);
	}

	// Open the file to send back.
	std::string full_path = doc_root_ + request_path;
	std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
	if (!is)
	{
		rep = reply::stock_reply(reply::not_found);
		return;
	}

	// Fill out the reply to be sent to the client.
	rep.status = reply::ok;
	char buf[512];
	while (is.read(buf, sizeof(buf)).gcount() > 0)
		rep.content.append(buf, is.gcount());

	rep.headers.resize(4);
	rep.headers[0].name = "Content-Length";
	rep.headers[0].value = std::to_string(rep.content.size());
	rep.headers[1].name = "Content-Type";
	rep.headers[1].value = mime_types::extension_to_type(extension);
	rep.headers[2].name = "Access-Control-Allow-Origin";
    rep.headers[2].value = "*";
	rep.headers[3].name = "Access-Control-Allow-Headers";
    rep.headers[3].value = "Origin, X-Requested-With, Content-Type, Accept";
}

bool request_handler::url_decode(const std::string& in, std::string& out)
{
    out.clear();
    out.reserve(in.size());
    for (std::size_t i = 0; i < in.size(); ++i)
    {
        if (in[i] == '%')
        {
            if (i + 3 <= in.size())
            {
                int value = 0;
                std::istringstream is(in.substr(i + 1, 2));
                if (is >> std::hex >> value)
                {
                    out += static_cast<char>(value);
                    i += 2;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        else if (in[i] == '+')
        {
            out += ' ';
        }
        else
        {
            out += in[i];
        }
    }

    return true;
}

} // namespace server
} // namespace http
} // namespace openrc.
