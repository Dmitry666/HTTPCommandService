#include "examplecontroller.h"

using namespace std;
namespace http {

CONTROLLER_REGISTERIMPL(ExampleController)
bool ExampleController::Validate(const SessionId& sessionId, const ControllerArguments& arguments) const
{
    return true;
}

CONTROLLER_ACTIONVALIDATEIMPL(ExampleController, DoAction, "DoAction", "Example action.")
bool ExampleController::DoActionValidate(const SessionId& sessionId, const ControllerArguments& arguments) const
{
    return true;
}

void ExampleController::DoAction(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& content)
{
    content.append("Example Controller: Hello. I am work.\n");
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

}
