#include "controller.h"
#include <algorithm>
#include "service-config.h"
#include <iostream>

using namespace std;

namespace openrc {

void ControllerOutput::Append(const std::string& value)
{
	_body.append(value);
}

string ToLower(const string& text)
{
    string textLower = text;
    transform(textLower.begin(), textLower.end(), textLower.begin(), ::tolower);
    return textLower;
}

IController::IController(const string& name, const string& description)
    : _name(name)
    , _description(description)
	, _enable(true)
{
	_enable = ServiceConfig::Instance().GetBoolean(_name, "enable", true);
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
#ifndef _MSC_VER
__attribute__ ((init_priority (101)))
#endif
;

void ControllerManager::RegisterController(IController* controller)
{
	controller->Construct();
    _controllers.insert(make_pair(ToLower(controller->ClassName()), controller));
    //controller->RegisterMethods();

	cout << "Register Controller: " << controller->ClassName() << endl;
}

IController* ControllerManager::FindController(const string& name)
{
    auto it = _controllers.find(ToLower(name));
    if(it != _controllers.end())
    {
		IController* controller = it->second;
        return controller->IsEnable() ? controller : nullptr;
    }

    return nullptr;
}

IController* ControllerManager::GetController(const std::string& name)
{
    auto it = _controllers.find(ToLower(name));
    if(it != _controllers.end())
    {
		IController* controller = it->second;
        return controller;
    }

    return nullptr;
}

const std::map<std::string, IController*>& ControllerManager::GetControllers() 
{
	return _controllers;
}

//ControllerManager& ControllerManager::Instance()
//{}

} // End http.
