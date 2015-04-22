#include "controller.h"


namespace http {

IController::ControllerMethod IController::FindMethod(const std::string& name)
{
    auto it = _methods.find(name);
    if(it != _methods.end())
    {
        return it->second;
    }

    return ControllerMethod();
}

//
std::map<std::string, IController*> ControllerManager::_controllers;

void ControllerManager::RegisterController(IController* controller)
{
    _controllers.insert(std::make_pair(controller->ClassName(), controller));
    controller->RegisterMethods();
}

IController* ControllerManager::FindController(const std::string& name)
{
    auto it = _controllers.find(name);
    if(it != _controllers.end())
    {
        return it->second;
    }

    return nullptr;
}

//ControllerManager& ControllerManager::Instance()
//{}

}
