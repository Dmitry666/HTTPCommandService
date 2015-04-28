#include <iostream>
#include <string>

//#include <boost/asio.hpp>
#include "httpservice.h"

using namespace http;

std::string _address = "0.0.0.0";
std::string _port = "10000";
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

	
    HttpService service(argc, argv);

	std::cout << "Start HTTP Service: " << _address << ":" << _port << ", " << _root;
    service.Start(_address, _port, _root);
    service.Join();

    return 0;
}
