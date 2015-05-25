#include "examplecontroller.h"

using namespace std;
namespace openrc {

CONTROLLER_REGISTERIMPL(ExampleController)
bool ExampleController::Construct()
{
    return true;
}

bool ExampleController::Validate(SessionWeak session, const ControllerArguments& arguments)
{
    UNUSED(session)
    UNUSED(arguments)

    return true;
}

CONTROLLER_ACTIONVALIDATEIMPL(ExampleController, DoAction, "DoAction", "Example action.")
bool ExampleController::DoActionValidate(SessionWeak session, const ControllerArguments& arguments)
{
    UNUSED(session)
    UNUSED(arguments)

    return true;
}

void ExampleController::DoAction(SessionWeak sessionWeak, const ControllerArguments& arguments, ControllerOutput& content)
{
	auto session = sessionWeak.lock();

    content.append("Example Controller: Hello. I am work.\n");
    content.append("User: " + std::to_string(session->Id.Id) + ".\n");

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
