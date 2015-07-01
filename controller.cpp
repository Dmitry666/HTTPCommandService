#include "controller.h"
#include <algorithm>
#include "service-config.h"
#include <iostream>
#include <assert.h>

using namespace std;

namespace openrc {

void ControllerOutput::Append(const string& value)
{
	_body.append(value);
}

string ToLower(const string& text)
{
    string textLower = text;
    transform(textLower.begin(), textLower.end(), textLower.begin(), ::tolower);
    return textLower;
}

ControllerMethod& ControllerMethod::operator ()(class IController* obj,
                                SessionWeak session,
                                const ControllerArguments& arguments,
                                ControllerOutput& contents)
{
	obj->BindCurrentMethod(this);
    this->Execute(obj, session, arguments, contents);
	obj->UnBindCurrentMethod(this);
    return (*this);
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

void IController::BindCurrentMethod(ControllerMethodRef method)
{
	assert(_currentMethod == nullptr);
	_currentMethod = method;
}

void IController::UnBindCurrentMethod(ControllerMethodRef method)
{
	assert(_currentMethod == method);
	_currentMethod = nullptr;
}

//
map<string, IController*> ControllerManager::_controllers
#ifndef _MSC_VER
__attribute__ ((init_priority (101)))
#endif
;

void ControllerManager::RegisterController(IController* controller)
{
    //controller->Construct();
    _controllers.insert(make_pair(ToLower(controller->ClassName()), controller));
    //controller->RegisterMethods();

	cout << "Register Controller: " << controller->ClassName() << endl;
}

void ControllerManager::ConstructControllers()
{
    for(auto& pair : _controllers)
    {
        IController* controller = pair.second;
        controller->Construct();
    }

    cout << "Contruct controllers." << endl;
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

IController* ControllerManager::GetController(const string& name)
{
    auto it = _controllers.find(ToLower(name));
    if(it != _controllers.end())
    {
		IController* controller = it->second;
        return controller;
    }

    return nullptr;
}

const std::map<string, IController*>& ControllerManager::GetControllers() 
{
	return _controllers;
}

//ControllerManager& ControllerManager::Instance()
//{}

} // End http.
