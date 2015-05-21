#include "usercontroller.h"

using namespace std;
namespace openrc {

CONTROLLER_REGISTERIMPL(UserController)
bool UserController::Construct()
{
    return true;
}

bool UserController::Validate(const SessionId& sessionId, const ControllerArguments& arguments)
{
    UNUSED(arguments)

    return _users.find(sessionId.Id) != _users.end();
}

CONTROLLER_ACTIONVALIDATEIMPL(UserController, Help, "Help", "Help information.")
bool UserController::HelpValidate(const SessionId& sessionId, const ControllerArguments& arguments)
{
    UNUSED(sessionId)
    UNUSED(arguments)

    return true;
}

void UserController::Help(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& content)
{
    UNUSED(sessionId)
    UNUSED(arguments)

    content.append("User/Login?name=user&password=qwerty.\n");
}

CONTROLLER_ACTIONVALIDATEIMPL(UserController, Login, "Login", "Authorization user.")
bool UserController::LoginValidate(const SessionId& sessionId, const ControllerArguments& arguments)
{
    UNUSED(sessionId)
    UNUSED(arguments)

    return true;
}

void UserController::Login(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& content)
{
    const std::string login = arguments["name"];
    const std::string password = arguments["password"];

    if( login.empty() || password.empty())
    {
        content.append("Incorrect login or password.\n");
        return;
    }

    if(_users.find(sessionId.Id) != _users.end())
    {
        content.append("User already authorizated.\n");
        content.append("Please logout.\n");
        return;
    }

    content.append("Hello user: " + login + ".\n");
    content.append("Session: " + std::to_string(sessionId.Id) + ".\n");

    User user;
    user.Login = login;
    user.Password = password;

    _users.insert(std::make_pair(sessionId.Id, user));
}

CONTROLLER_ACTIONIMPL(UserController, DoAction, "DoAction", "Action.")
void UserController::DoAction(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& content)
{
    UNUSED(arguments)

    auto it = _users.find(sessionId.Id);
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
void UserController::Logout(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& content)
{
    UNUSED(arguments)

    auto it = _users.find(sessionId.Id);
    if(it == _users.end())
    {
        content.append("Error\n");
        return;
    }

    _users.erase(it);
    content.append("Ok\n");
}

}
