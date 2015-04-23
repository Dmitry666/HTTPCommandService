#include "defaultcontroller.h"

using namespace std;

namespace http {

CONTROLLER_REGISTERIMPL(DefaultController)

bool DefaultController::Validate(const SessionId& sessionId, const ControllerArguments& arguments) const
{
    return true;
}

CONTROLLER_ACTIONVALIDATEIMPL(DefaultController, Test, "Test", "Action from test.")
bool DefaultController::TestValidate(const SessionId& sessionId, const ControllerArguments& arguments) const
{
    return true;
}

void DefaultController::Test(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& content)
{
    content.append("Default Controller: Hello. I am work.\n");
    content.append("User: " + std::to_string(sessionId.Id) + ".\n");

    for(auto& pair : arguments)
    {
        content.append(pair.first + " = " + pair.second + "\n");
    }

    content.append("\n");

    auto it = arguments.find("text");
    if(it != arguments.end())
    {
        if( it->second == "Hello" )
        {
            content.append("Hello User.\n");
        }
    }
}

} // End http.
