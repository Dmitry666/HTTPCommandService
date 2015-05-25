//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "tcp_request_handler.hpp"
#include "tcp_reply.hpp"
#include "tcp_request.hpp"

#include <fstream>
#include <sstream>
#include <string>

#include "../controller.h"
#include "../sessionmanager.h"

using namespace std;

namespace openrc {
namespace tcpnative {

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
	const string& controllerName = req.controller;
	const string& method = req.method;
	const map<string, string> argumentsMap = req.arguments;

	IController* icontroller = ControllerManager::FindController(controllerName);
	if(icontroller == nullptr)
	{
		//handle_request_page(request_path, req, rep);
		return;
	}

	if( !icontroller->BeginAction() )
	{
		//rep = reply::stock_reply(reply::not_found);
		return;
	}

	ControllerMethodRef methodRef = icontroller->FindMethod(method);
	if( methodRef == nullptr )
	{
		//rep = reply::stock_reply(reply::not_found);
		return;
	}

	SessionKey sessionKey;
	SessionWeak sessionWeak = sessionKey.empty() ?
				SessionManager::NewSession() :
				SessionManager::FindSessionByKey(sessionKey);

	if(sessionWeak.expired())
	{
		sessionWeak = SessionManager::NewSession();
	}

	SessionShared session = sessionWeak.lock();

	bool validate = methodRef->IsValidateMethod() ? methodRef->Validate(icontroller, sessionWeak, argumentsMap) :
			icontroller->Validate(sessionWeak, argumentsMap);

	if(!validate)
	{
		//rep = reply::stock_reply(reply::not_found);
		return;
	}

	ControllerOutput output;
	(*methodRef)(icontroller, sessionWeak, argumentsMap, output);

	//rep.content = output.GetBody();
	icontroller->EndAction();

	// rep
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

} // namespace tcpnative
} // namespace openrc.