#include "helpcontroller.h"

using namespace std;

namespace http {

CONTROLLER_REGISTERIMPL(HelpController)

CONTROLLER_ACTIONIMPL(HelpController, ShowControllers, "ShowControllers", "Print all controller with their actions.")
void HelpController::ShowControllers(const ControllerArguments& arguments, ControllerOutput& content)
{
    enum OutputFormat
    {
        OF_Text,
        OF_Json,
        OF_Xml
    };

    OutputFormat outputFormat = OutputFormat::OF_Text;
    auto it = arguments.find("format");
    if(it = arguments.end())
    {
        const string formatText = it->second;

        if(formatText == "text")
            outputFormat = OutputFormat::OF_Text;
        else if(formatText == "json")
            outputFormat = OutputFormat::OF_Json;
        else if(formatText == "xml")
            outputFormat = OutputFormat::OF_Xml;
    }

    content.append("Controllers:\n\n");

    for(auto& controllerPair : ControllerManager::GetControllers())
    {
        content.append("\t" +
                       controllerPair.second->GetName() + " - " +
                       controllerPair.second->GetDescription() +
                       "\n");

        for(auto& actionPair : controllerPair.second->GetActions())
        {
            content.append("\t\t" +
                           actionPair.second->GetName() + " - " +
                           actionPair.second->GetDescription() +
                           "\n");
        }
    }
}

} // End http.
