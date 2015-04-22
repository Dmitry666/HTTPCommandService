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
#include "controller.h"

using namespace std;

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
    // Decode url to path.
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
        rep = reply::stock_reply(reply::not_found);
        return;
    }

    const string controllerName = controllerBlocks[0];
    const string commandName = controllerBlocks[1];

    const vector<string> arguments = split(blockB, '&');

    IController* icontroller = ControllerManager::FindController(controllerName);
    if(icontroller == nullptr)
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

    (*methodRef)(icontroller, argumentsMap, rep.content);

    /*
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
    */

    rep.headers.resize(2);
    rep.headers[0].name = "Content-Length";
    rep.headers[0].value = std::to_string(rep.content.size());
    rep.headers[1].name = "Content-Type";
    //rep.headers[1].value = mime_types::extension_to_type(extension);
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
