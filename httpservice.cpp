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

HttpService::HttpService(int argc, char* argv[])
{
    ModuleManager::Instance().LoadModulesFromFolder("./");
    ModuleManager::Instance().LoadModulesFromFolder(_rootDir);

    ModuleManager::Instance().InitializeAll();
#ifdef WITH_JAVASCRIPT
	JavascriptManager::Instance().Initialize(argc, argv);
#endif
}

HttpService::~HttpService()
{
#ifdef WITH_JAVASCRIPT
	JavascriptManager::Instance().Shutdown();
#endif
    ModuleManager::Instance().ShutdownAll();
}

bool HttpService::Start(const std::string& address,
        const std::string& port,
        const std::string& root)
{
	_address = address;
	_port = port;
	_rootDir = root;

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
        _s = new server::server(_address, _port, _rootDir);

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
