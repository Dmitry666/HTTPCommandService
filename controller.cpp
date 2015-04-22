#include "controller.h"
#include <algorithm>

namespace http {

ControllerMethodRef IController::FindMethod(const std::string& name)
{
    std::string nameLower = name;
    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

    auto it = _methods.find(nameLower);
    if(it != _methods.end())
    {
        return it->second;
    }

    return nullptr;
}

void IController::RegisterMethod(const std::string& name, ControllerMethodRef method)
{
    std::string nameLower = name;
    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

    _methods.insert(std::make_pair(nameLower, method));
}



//
std::map<std::string, IController*> ControllerManager::_controllers
__attribute__ ((init_priority (101)));

void ControllerManager::RegisterController(IController* controller)
{
    std::string nameLower = controller->ClassName();
    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

    _controllers.insert(std::make_pair(nameLower, controller));
    controller->RegisterMethods();
}

IController* ControllerManager::FindController(const std::string& name)
{
    std::string nameLower = name;
    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

    auto it = _controllers.find(nameLower);
    if(it != _controllers.end())
    {
        return it->second;
    }

    return nullptr;
}

//ControllerManager& ControllerManager::Instance()
//{}

}
