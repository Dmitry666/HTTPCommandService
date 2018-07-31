#include "httpservice.h"
#include <thread>


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

#include "http/server.hpp"

#ifdef WITH_WEBSOCKET
#include "websocket/web_server.hpp"
#endif

#ifdef WITH_TCPNATIVE
#include "tcpnative/tcp_server.hpp"
#endif

#include "controller.h"
#include "modulemanager.h"
#include "javascriptmanager.h"

#include <iostream>

//using namespace openrc::http;
using namespace std;

std::vector<shared_ptr<thread>> _threads;


namespace openrc {

vector<weak_ptr<base_server>> _servers;

void RunServer(shared_ptr<base_server> _s)
{
    try
    {
        // Initialise the server.
        //_s = new T(address, port, arguments["root"]);

        // Run the server until stopped.
        _s->run();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }
}

HttpService::HttpService(const HttpServiceArguments& arguments)
	: _arguments(arguments)
{
    ControllerManager::ConstructControllers();
    ModuleManager::Instance().LoadModulesFromFolder("./");

	const std::string rootDir = _arguments["root"];
	ModuleManager::Instance().LoadModulesFromFolder(rootDir.empty() ? "./modules" : rootDir);


    ModuleManager::Instance().InitializeAll();
#ifdef WITH_JAVASCRIPT
	const std::string jsPath = _arguments["js"];
	JavascriptManager::Instance().Initialize(jsPath.empty() ? "./js" : jsPath);
#endif
}

HttpService::~HttpService()
{
	_threads.clear();
	_servers.clear();

#ifdef WITH_JAVASCRIPT
	JavascriptManager::Instance().Shutdown();
#endif
    ModuleManager::Instance().ShutdownAll();
}

bool HttpService::Start(const string& address, const string& port)
{
	_address = address;
	_port = port;

	// Jttp.
	std::size_t thread_pool_size = _arguments["thread_pool_size"].empty() ? 1 : stoi(_arguments["thread_pool_size"]);
	auto httpServer = make_shared<http::server::server>(_address, _port, _arguments["root"], thread_pool_size);
	_servers.push_back(httpServer);

    auto thread = make_shared<std::thread>(&RunServer, httpServer);
	_threads.push_back(thread);

	// WebSocket
#ifdef WITH_WEBSOCKET
	auto wsServer = make_shared<websocket::web_server>(_address, "80", _arguments["root"]);
	_servers.push_back(wsServer);

    auto wsThread = make_shared<std::thread>(&RunServer, wsServer);
	_threads.push_back(wsThread);
#endif

    return true;
}

bool HttpService::Stop()
{
	for(auto& serverWeak : _servers)
	{
		auto server = serverWeak.lock();
		if(server)
		{
			server->stop();
		}
	}

    return true;
}

bool HttpService::Join(float time)
{
	for(auto& thread : _threads)
	{
		thread->join();
	}

    return true;
}

#if 0
void HttpService::Run()
{

    try
    {
        // Initialise the server.
        _s = new server::server(_address, _port, _arguments["root"]);

        // Run the server until stopped.
        _s->run();
    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }

    delete _s;
    _s = nullptr;

}
#endif

} // End http.
