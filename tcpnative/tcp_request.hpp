//
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef TCP_REQUEST_HPP
#define TCP_REQUEST_HPP

#include <string>
#include <vector>
#include <map>

namespace openrc {
namespace tcpnative {

/// A request received from a client.
struct request
{
    std::string controller;
    std::string method;

    std::map<std::string, std::string> arguments;
};

} // namespace tcpnative
} // namespace openrc.

#endif // TCP_REQUEST_HPP
