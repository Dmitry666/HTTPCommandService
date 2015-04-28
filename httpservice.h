#ifndef HTTPSERVICE_H
#define HTTPSERVICE_H

#include "common.h"


namespace http {

class HttpService
{
public:
    HCORE_API HttpService(int argc, char* argv[]);
    HCORE_API ~HttpService();

    HCORE_API bool Start(const std::string& address,
        const std::string& port,
        const std::string& root);
    HCORE_API bool Stop();
    HCORE_API bool Join(float time = 0.f);

private:
    void Run();

private:
    std::string _address;
    std::string _port;
    std::string _rootDir;
};

} // End http.

#endif // HTTPSERVICE_H
