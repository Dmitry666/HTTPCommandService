#include "asyncmodule.h"
#include "asynccontroller.h"

using namespace openrc;

void AsyncModule::InitialiseModule()
{}

void AsyncModule::ShutdownModule()
{}

IModule* GetModule()
{
    return new AsyncModule();
}

