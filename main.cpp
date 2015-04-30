#include <iostream>
#include <string>

#include "httpservice.h"

using namespace http;
using namespace std;

string _address = "0.0.0.0";
string _port = "10000";
string _root = "./";

int main(int argc, char* argv[])
{
	HttpServiceArguments arguments;
    if (argc == 4)
    {
        _address = argv[1];
        _port = argv[2];

		arguments.Push("root", argv[3]);
        //std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
        //std::cerr << "  For IPv4, try:\n";
        //std::cerr << "    receiver 0.0.0.0 80 .\n";
        //std::cerr << "  For IPv6, try:\n";
        //std::cerr << "    receiver 0::0 80 .\n";

        //return 1;
    }

    HttpService service(arguments);

	cout << "Start HTTP Service: " << _address << ":" << _port << ", " << _root << endl;
    service.Start(_address, _port);

	bool bExit = false;
	do
	{
		string command;
		getline (std::cin, command);
		transform(command.begin(), command.end(), command.begin(), ::tolower);

		if (command == "stop" || command == "exit")
			bExit = true;
	} 
	while (!bExit);

	service.Stop();
    service.Join();

    return 0;
}
