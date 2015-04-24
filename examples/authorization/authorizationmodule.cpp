#include "authorizationmodule.h"
#include "usercontroller.h"

using namespace http;

void AuthorizationModule::InitialiseModule()
{}

void AuthorizationModule::ShutdownModule()
{}

IModule* GetModule()
{
    return new AuthorizationModule();
}

