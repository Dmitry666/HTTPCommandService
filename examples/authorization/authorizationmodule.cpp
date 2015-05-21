#include "authorizationmodule.h"
#include "usercontroller.h"

using namespace openrc;

void AuthorizationModule::InitialiseModule()
{}

void AuthorizationModule::ShutdownModule()
{}

IModule* GetModule()
{
    return new AuthorizationModule();
}

