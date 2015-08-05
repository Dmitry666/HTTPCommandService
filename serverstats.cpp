#include "serverstats.h"

#include <iostream>
#include <functional>
#include <chrono>


namespace openrc {

ServerStats& ServerStats::Instance()
{
    static ServerStats serverStats;
    return serverStats;
}

ServerStats::ServerStats()
    : _active(true)

    , _nbRequests(0)
    , _nbRequestBytes(0)
    , _nbResponses(0)
    , _nbResponseBytes(0)

    , _requestPerSecond(0.0)
    , _requestBytesPerSecond(0.0)
    , _responsePerSecond(0.0)
    , _responseBytesPerSecond(0.0)
{
    _thread = std::thread(std::bind(&ServerStats::Run, this));
}

ServerStats::~ServerStats()
{
    _active = false;
    _thread.join();
}

void ServerStats::NewRequest(int nbBytes)
{
    ++_nbRequests;
    _nbRequestBytes += nbBytes;
}

void ServerStats::NewResponse(int nbBytes)
{
    ++_nbResponses;
    _nbResponseBytes += nbBytes;
}

void ServerStats::Run()
{
    do
    {
        int64_t lastNbRequests = _nbRequests;
        int64_t lastNbRequestBytes = _nbRequestBytes;
        int64_t lastNbResponses = _nbResponses;
        int64_t lastNbResponseBytes = _nbResponseBytes;


        std::this_thread::sleep_for(std::chrono::seconds(5));

        int64_t deltaNbRequests = _nbRequests - lastNbRequests;
        int64_t deltaNbRequestBytes = _nbRequestBytes - lastNbRequestBytes;
        int64_t deltaNbResponses = _nbResponses - lastNbResponses;
        int64_t deltaNbResponseBytes = _nbResponseBytes - lastNbResponseBytes;

        _requestPerSecond = double(deltaNbRequests) / 5.0;
        _requestBytesPerSecond = double(deltaNbRequestBytes) / 5.0;
        _responsePerSecond = double(deltaNbResponses) / 5.0;
        _responseBytesPerSecond = double(deltaNbResponseBytes) / 5.0;
    }
    while(_active);
}

} // End openrc.
