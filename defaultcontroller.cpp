#include "defaultcontroller.h"

using namespace std;

namespace http {

CONTROLLER_REGISTERIMPL(DefaultController)

CONTROLLER_ACTIONIMPL(DefaultController, Test, "Test", "Action from test.")
void DefaultController::Test(const ControllerArguments& arguments, ControllerOutput& content)
{
    content.append("Default Controller: Hello. I am work.\n");
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
