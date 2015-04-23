#ifndef HTTP_MODULE_H
#define HTTP_MODULE_H

#include "common.h"

namespace http {

class IModule
{
public:
    virtual ~IModule()
    {}

    virtual const char* GetModuleName() const = 0;

    virtual void InitialiseModule() = 0;
    virtual void ShutdownModule() = 0;
};

} // end http.

#endif // MODULE

