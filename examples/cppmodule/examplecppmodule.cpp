#include "examplecppmodule.h"
#include "examplecontroller.h"

using namespace http;

void ExampleCppModule::InitialiseModule()
{}

void ExampleCppModule::ShutdownModule()
{}

IModule* GetModule()
{
    return new ExampleCppModule();
}

