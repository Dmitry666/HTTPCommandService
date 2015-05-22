//
// reply.hpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef WEB_REPLY_HPP
#define WEB_REPLY_HPP

#include "../common-private.h"
#include <string>
#include <vector>

#ifndef _MSC_VER
// save diagnostic state
#pragma GCC diagnostic push
// turn off the specific warning. Can also use "-Wall"
#pragma GCC diagnostic ignored "-Wall"
//#pragma GCC diagnostic ignored "-Wunused-variable"
//#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#define BOOST_CONFIG_WARNING_DISABLE_HPP
#pragma GCC system_header
#endif

#include <boost/asio.hpp>

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

namespace openrc {
namespace websocket {

/// A reply to be sent to a client.
struct reply
{
    std::vector<std::string> contents;

    std::vector<boost::asio::const_buffer> to_buffers();
};

} // namespace websocket
} // namespace openrc.

#endif // WEB_REPLY_HPP
