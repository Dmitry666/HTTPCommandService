#ifndef EXAMPLECPPMODULE_H
#define EXAMPLECPPMODULE_H

#include <module.h>

#ifndef _Export
#define _Export __declspec(dllexport)
#endif

class DataBaseModule : public openrc::IModule
{
public:
    DataBaseModule()
    {}

    virtual const char* GetModuleName() const override { return "DataBaseModule"; }

    virtual void InitialiseModule() override;
    virtual void ShutdownModule() override;
};

extern "C"
_Export openrc::IModule* GetModule();

#endif // EXAMPLECPPMODULE_H
