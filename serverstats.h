#ifndef SERVERSTATS_H
#define SERVERSTATS_H

#include "common.h"
#include <thread>

namespace openrc {

class ServerStats
{
private:
    ServerStats();
    ~ServerStats();

public:
    void NewRequest(int nbBytes);
    void NewResponse(int nbBytes);

    int64_t GetNbRequest() const {return _nbRequests;}
    int64_t GetNbRequestBytes() const {return _nbRequestBytes;}
    int64_t GetNbResponse() const {return _nbResponses;}
    int64_t GetNbResponseBytes() const {return _nbResponseBytes;}

    double GetRequestPerSecond() const {return _requestPerSecond;}
    double GetRequestBytesPerSecond() const {return _requestBytesPerSecond;}
    double GetResponsePerSecond() const {return _responsePerSecond;}
    double GetResponseBytesPerSecond() const {return _responseBytesPerSecond;}

    static ServerStats& Instance();

private:
    void Run();

protected:
    bool _active;

    int64_t _nbRequests;
    int64_t _nbRequestBytes;
    int64_t _nbResponses;
    int64_t _nbResponseBytes;

    double _requestPerSecond;
    double _requestBytesPerSecond;
    double _responsePerSecond;
    double _responseBytesPerSecond;

    std::thread _thread;
};

} // End openrc.

#endif // SERVERSTATS_H
