//
// reply.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "web_reply.hpp"
#include <string>

using namespace std;

namespace openrc {
namespace websocket {

std::vector<boost::asio::const_buffer> reply::to_buffers()
{
    std::vector<boost::asio::const_buffer> buffers;
    for (const string& content : contents)
    {
        buffers.push_back(boost::asio::buffer(content));
    }
    return buffers;
}

} // namespace websocket
} // namespace openrc.