#include "httpservice.h"
#include <thread>

#include <boost/asio.hpp>
#include "server.hpp"
#include "modulemanager.h"

using namespace http;
using namespace std;

thread _thread;
http::server::server* _s;

namespace http {

HttpService::HttpService(
    const std::string& address,
    const std::string& port,
    const std::string& root)
    : _address(address)
    , _port(port)
    , _rootDir(root)
{
    ModuleManager::Instance().LoadModulesFromFolder("./");
    ModuleManager::Instance().LoadModulesFromFolder(_rootDir);

    ModuleManager::Instance().InitializeAll();
}

HttpService::~HttpService()
{
    ModuleManager::Instance().ShutdownAll();
}

bool HttpService::Start()
{
    _thread = std::thread(&HttpService::Run, this);
}

bool HttpService::Stop()
{
    //_s->S
}

bool HttpService::Join(float time)
{
    _thread.join();
}

void HttpService::Run()
{
    // Initialise the server.
    _s = new server::server(_address, _port, _rootDir);

    // Run the server until stopped.
    _s->run();

    delete _s;
    _s = nullptr;
}

} // End http.