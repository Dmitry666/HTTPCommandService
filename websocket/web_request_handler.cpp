//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "web_request_handler.hpp"
#include "web_reply.hpp"
#include "web_request.hpp"

#include <sstream>
#include <string>




#include "../controller.h"
#include "../sessionmanager.h"

#include <json/json.h>
#include <json/reader.h>

using namespace std;

namespace openrc {
namespace websocket {

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
/*
[
	{
		command: "Project.GetFile",
		params: {
			key: value,
			key2: value2,
			...
		}
	}
]
*/
	struct CommandBody
	{
		string Command;
		map<string, string> ArgumentsMap;

		CommandBody()
		{}

		CommandBody(const CommandBody& cb)
			: Command(cb.Command)
			, ArgumentsMap(cb.ArgumentsMap)
		{}
	};
	vector<CommandBody> commands;

	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
	bool parsingSuccessful = reader.parse( req.body, root );
	if ( !parsingSuccessful )
	{
		// report to the user the failure and their locations in the document.
		//std::cout  << "Failed to parse configuration\n"
		//		   << reader.getFormattedErrorMessages();
		rep = reply::stock_reply(reply::not_found);
		return;
	}

	for (int index = 0; index < root.size(); ++index) 
	{ 
        Json::Value commandObjectValue = root[index];
		if(commandObjectValue.isObject())
		{
			string commandValue = commandObjectValue["command"].asString();

			CommandBody command;
			command.Command = commandValue;

			Json::Value paramsValue = commandObjectValue["params"];
			if(paramsValue.isObject())
			{
				for(const string& member : paramsValue.getMemberNames())
				{
					const string value = paramsValue[member].asString();
					command.ArgumentsMap.insert(std::make_pair(member, value));
				}
			}

			commands.push_back(command);
		}
		
    }

	rep.body = "[";
	for(CommandBody& cb : commands)
	{	
		vector<string> blocks = split(cb.Command, '.');
		if(blocks.size() != 2)
		{
			rep.body += std::string("{\n") +
				"\"command\":\"" + cb.Command + "\",\n" +
				"\"data\":\"Error: Bad command.\"\n"
				"}";
			continue;
		}

		const string& controllerName = blocks[0];
		const string& method = blocks[1];
		const map<string, string>& argumentsMap = cb.ArgumentsMap;

		auto icontroller = ControllerManager::FindController(controllerName);
		if(icontroller == nullptr)
		{
			rep.body += string("{\n") +
				"\"command\":\"" + cb.Command + "\",\n" +
				"\"data\":\"Error: Not found controller.\"\n"
				"}";
			continue;
		}

		if( !icontroller->BeginAction() )
		{
			rep.body += string("{\n") +
				"\"command\":\"" + cb.Command + "\",\n" +
				"\"data\":\"Error: Begin action.\"\n"
				"}";
			continue;
		}

		ControllerMethodRef methodRef = icontroller->FindMethod(method);
		if( methodRef == nullptr )
		{
			rep.body += string("{\n") +
				"\"command\":\"" + cb.Command + "\",\n" +
				"\"data\":\"Error: Not found method.\"\n"
				"}";
			continue;
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
			rep.body += string("{\n") +
				"\"command\":\"" + cb.Command + "\",\n" +
				"\"data\":\"Error: Bad validation.\"\n"
				"}";

			icontroller->EndAction();
			continue;
		}

		ControllerOutput output;
		(*methodRef)(icontroller, sessionWeak, argumentsMap, output);

		rep.body += string("{\n") +
			"\"command\":\"" + cb.Command + "\",\n" +
			"\"data\":\"" + quoted(output.GetBody()) + "\"\n"
			"}";

		if(&cb != &commands.back())
		{
			rep.body += ",\n";
		}

		icontroller->EndAction();
	}
	rep.body += "]";
}

void request_handler::handle_async_response(const request& req, reply& rep)
{
	SessionKey sessionKey;
	SessionWeak sessionWeak = SessionManager::FindSessionByKey(sessionKey);

	SessionShared session = sessionWeak.lock();

	//

	//
}

bool request_handler::url_decode(const string& in, string& out)
{
    out.clear();
    out.reserve(in.size());
    for (size_t i = 0; i < in.size(); ++i)
    {
        if (in[i] == '%')
        {
            if (i + 3 <= in.size())
            {
                int value = 0;
                istringstream is(in.substr(i + 1, 2));
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

} // namespace websocket
} // namespace openrc.
