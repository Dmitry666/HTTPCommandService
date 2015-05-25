#include "defaultcontroller.h"

using namespace std;

namespace openrc {

CONTROLLER_REGISTERIMPL(DefaultController)

bool DefaultController::Construct()
{
    return true;
}

bool DefaultController::Validate(SessionWeak session, const ControllerArguments& arguments) 
{
    UNUSED(session)
    UNUSED(arguments)

    return true;
}

CONTROLLER_ACTIONVALIDATEIMPL(DefaultController, Test, "Test", "Action from test.")
bool DefaultController::TestValidate(SessionWeak session, const ControllerArguments& arguments)
{
    UNUSED(session)
    UNUSED(arguments)

    return true;
}

void DefaultController::Test(SessionWeak sessionWeak, const ControllerArguments& arguments, ControllerOutput& content)
{
	auto session = sessionWeak.lock();

    content.append("Default Controller: Hello. I am work.\n");
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

} // End http.
