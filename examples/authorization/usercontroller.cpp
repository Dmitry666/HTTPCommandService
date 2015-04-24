#include "usercontroller.h"

using namespace std;
namespace http {

CONTROLLER_REGISTERIMPL(UserController)
bool UserController::Validate(const SessionId& sessionId, const ControllerArguments& arguments) const
{
    return _authorizated;
}

CONTROLLER_ACTIONVALIDATEIMPL(UserController, Help, "Help", "Help information.")
bool UserController::HelpValidate(const SessionId& sessionId, const ControllerArguments& arguments) const
{
    return true;
}

void UserController::Help(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& content)
{
    content.append("User/Login?name=\"user\"&password=\"qwerty\".\n");
}

CONTROLLER_ACTIONVALIDATEIMPL(UserController, Login, "Login", "Authorization user.")
bool UserController::LoginValidate(const SessionId& sessionId, const ControllerArguments& arguments) const
{
    return true;
}

void UserController::Login(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& content)
{
    if( arguments["name"] != "user" || arguments["password"] != "qwerty")
    {
        content.append("Incorrect login or password.\n");
        return;
    }

    if(_authorizated)
    {
        content.append("User already authorizated.\n");
        content.append("Please logout.\n");
    }

    content.append("Hello user: " + arguments["name"] + ".\n");
    content.append("Session: " + std::to_string(sessionId.Id) + ".\n");
}

CONTROLLER_ACTIONIMPL(UserController, DoAction, "DoAction", "Action.")
void UserController::DoAction(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& content)
{
    content.append("Hello. I am work.\n");
    content.append("User: " + std::to_string(sessionId.Id) + ".\n");
}

CONTROLLER_ACTIONIMPL(UserController, Logout, "DoAction", "Deauthorization user.")
void UserController::Logout(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& content)
{
    _authorizated = false;
}

}
