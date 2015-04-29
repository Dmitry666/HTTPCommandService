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

#include "server.hpp"
#include "modulemanager.h"
#include "javascriptmanager.h"

using namespace http;
using namespace std;

thread _thread;
http::server::server* _s;

namespace http {

HttpService::HttpService(const HttpServiceArguments& arguments)
	: _arguments(arguments)
{
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
#ifdef WITH_JAVASCRIPT
	JavascriptManager::Instance().Shutdown();
#endif
    ModuleManager::Instance().ShutdownAll();
}

bool HttpService::Start(const std::string& address, const std::string& port)
{
	_address = address;
	_port = port;

    _thread = std::thread(&HttpService::Run, this);
    return true;
}

bool HttpService::Stop()
{
    //_s->S
    return true;
}

bool HttpService::Join(float time)
{
    _thread.join();
    return true;
}

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

} // End http.
