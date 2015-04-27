#ifndef HTTPSERVICE_H
#define HTTPSERVICE_H

#include "common.h"


namespace http {

class HttpService
{
public:
    HttpService(
        const std::string& address,
        const std::string& port,
        const std::string& root);
    ~HttpService();

    bool Start();
    bool Stop();
    bool Join(float time = 0.f);

private:
    void Run();

private:
    std::string _address;
    std::string _port;
    std::string _rootDir;
};

} // End http.

#endif // HTTPSERVICE_H
