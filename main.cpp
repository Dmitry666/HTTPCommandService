//
// main.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>

//#include <boost/asio.hpp>
//#include "server.hpp"
//#include "modulemanager.h"
#include "httpservice.h"

using namespace http;

std::string _address = "0.0.0.0";
std::string _port = "80";
std::string _root = "./";

int main(int argc, char* argv[])
{
    if (argc == 4)
    {
        _address = argv[1];
        _port = argv[2];
        _root = argv[3];

        //std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
        //std::cerr << "  For IPv4, try:\n";
        //std::cerr << "    receiver 0.0.0.0 80 .\n";
        //std::cerr << "  For IPv6, try:\n";
        //std::cerr << "    receiver 0::0 80 .\n";

        //return 1;
    }

    HttpService service(_address, _port, _root);
    service.Start();
    service.Join();

#if 0
    ModuleManager::Instance().LoadModulesFromFolder("./");
    ModuleManager::Instance().InitializeAll();

    try
    {
        // Check command line arguments.
        if (argc != 4)
        {
            std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
            std::cerr << "  For IPv4, try:\n";
            std::cerr << "    receiver 0.0.0.0 80 .\n";
            std::cerr << "  For IPv6, try:\n";
            std::cerr << "    receiver 0::0 80 .\n";
            return 1;
        }

        // Initialise the server.
        http::server::server s(argv[1], argv[2], argv[3]);

        // Run the server until stopped.
        s.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }
#endif

    return 0;
}
