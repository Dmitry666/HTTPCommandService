#ifndef EXAMPLECPPMODULE_H
#define EXAMPLECPPMODULE_H

#include <module.h>

#ifndef _Export
#define _Export __declspec(dllexport)
#endif

class ExampleCppModule : public openrc::IModule
{
public:
    ExampleCppModule()
    {}

    virtual const char* GetModuleName() const override { return "ExampleCppModule"; }

    virtual void InitialiseModule() override;
    virtual void ShutdownModule() override;
};

extern "C"
_Export openrc::IModule* GetModule();

#endif // EXAMPLECPPMODULE_H
