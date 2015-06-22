#ifndef HTTP_REQUEST_MODULE_H
#define HTTP_REQUEST_MODULE_H

#include <module.h>
#include <curl/curl.h>


#ifdef _WIN32
#ifndef _Export
#define _Export __declspec(dllexport)
#endif
#else
#define _Export
#endif

class HttpRequestModule : public openrc::IModule
{
public:
    HttpRequestModule()
    {}

    virtual const char* GetModuleName() const override { return "DataBaseModule"; }

    virtual void InitialiseModule() override;
    virtual void ShutdownModule() override;
};

extern "C"
_Export openrc::IModule* GetModule();

#endif // EXAMPLECPPMODULE_H
