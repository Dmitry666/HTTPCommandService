#include "examplecppmodule.h"
#include "examplecontroller.h"

using namespace openrc;

void ExampleCppModule::InitialiseModule()
{}

void ExampleCppModule::ShutdownModule()
{}

IModule* GetModule()
{
    return new ExampleCppModule();
}

