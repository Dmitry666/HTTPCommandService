#include "controller.h"
#include <algorithm>

using namespace std;

namespace http {

string ToLower(const string& text)
{
    string textLower = text;
    transform(textLower.begin(), textLower.end(), textLower.begin(), ::tolower);
    return textLower;
}

ControllerMethodRef IController::FindMethod(const string& name)
{
    auto it = _methods.find(ToLower(name));
    if(it != _methods.end())
    {
        return it->second;
    }

    return nullptr;
}

void IController::RegisterMethod(ControllerMethodRef method)
{
    _methods.insert(make_pair(ToLower(method->GetName()), method));
}



//
map<string, IController*> ControllerManager::_controllers
__attribute__ ((init_priority (101)));

void ControllerManager::RegisterController(IController* controller)
{
    _controllers.insert(make_pair(ToLower(controller->ClassName()), controller));
    controller->Construct();
    //controller->RegisterMethods();
}

IController* ControllerManager::FindController(const string& name)
{
    auto it = _controllers.find(ToLower(name));
    if(it != _controllers.end())
    {
        return it->second;
    }

    return nullptr;
}

//ControllerManager& ControllerManager::Instance()
//{}

} // End http.
