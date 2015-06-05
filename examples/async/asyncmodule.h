#ifndef EXAMPLEASYNC_H
#define EXAMPLEASYNC_H

#include <module.h>

#ifndef _Export
#define _Export __declspec(dllexport)
#endif

class AsyncModule : public openrc::IModule
{
public:
    AsyncModule()
    {}

    virtual const char* GetModuleName() const override { return "AsyncModule"; }

    virtual void InitialiseModule() override;
    virtual void ShutdownModule() override;
};

extern "C"
_Export openrc::IModule* GetModule();

#endif // EXAMPLECPPMODULE_H
