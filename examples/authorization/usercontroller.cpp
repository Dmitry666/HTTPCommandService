#include "usercontroller.h"

using namespace std;
namespace openrc {

CONTROLLER_REGISTERIMPL(UserController)
bool UserController::Construct()
{
    return true;
}

bool UserController::Validate(SessionWeak sessionWeak, const ControllerArguments& arguments)
{
	UNUSED(arguments)

	auto session = sessionWeak.lock();
    return _users.find(session->Id.Id) != _users.end();
}

CONTROLLER_ACTIONVALIDATEIMPL(UserController, Help, "Help", "Help information.")
bool UserController::HelpValidate(SessionWeak session, const ControllerArguments& arguments)
{
    UNUSED(session)
    UNUSED(arguments)

    return true;
}

void UserController::Help(SessionWeak session, const ControllerArguments& arguments, ControllerOutput& content)
{
    UNUSED(session)
    UNUSED(arguments)

    content.append("User/Login?name=user&password=qwerty.\n");
}

CONTROLLER_ACTIONVALIDATEIMPL(UserController, Login, "Login", "Authorization user.")
bool UserController::LoginValidate(SessionWeak session, const ControllerArguments& arguments)
{
    UNUSED(session)
    UNUSED(arguments)

    return true;
}

void UserController::Login(SessionWeak sessionWeak, const ControllerArguments& arguments, ControllerOutput& content)
{
	auto session = sessionWeak.lock();

    const std::string login = arguments["name"];
    const std::string password = arguments["password"];

    if( login.empty() || password.empty())
    {
        content.append("Incorrect login or password.\n");
        return;
    }

    if(_users.find(session->Id.Id) != _users.end())
    {
        content.append("User already authorizated.\n");
        content.append("Please logout.\n");
        return;
    }

    content.append("Hello user: " + login + ".\n");
    content.append("Session: " + std::to_string(session->Id.Id) + ".\n");

    User user;
    user.Login = login;
    user.Password = password;

    _users.insert(std::make_pair(session->Id.Id, user));
}

CONTROLLER_ACTIONIMPL(UserController, DoAction, "DoAction", "Action.")
void UserController::DoAction(SessionWeak sessionWeak, const ControllerArguments& arguments, ControllerOutput& content)
{
    UNUSED(arguments)

	auto session = sessionWeak.lock();

    auto it = _users.find(session->Id.Id);
    if(it == _users.end())
    {
        content.append("Exception.\n");
        return;
    }

    const std::string name = it->second.Login;

    content.append("Hello. I am work.\n");
    content.append("User " + name + ": DoAction.\n");
}

CONTROLLER_ACTIONIMPL(UserController, Logout, "Logout", "Deauthorization user.")
void UserController::Logout(SessionWeak sessionWeak, const ControllerArguments& arguments, ControllerOutput& content)
{
    UNUSED(arguments)

	auto session = sessionWeak.lock();

    auto it = _users.find(session->Id.Id);
    if(it == _users.end())
    {
        content.append("Error\n");
        return;
    }

    _users.erase(it);
    content.append("Ok\n");
}

}
