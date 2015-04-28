#include "helpcontroller.h"

#ifdef WITH_JSONCPP
#include <json/writer.h>
#endif

using namespace std;

namespace http {

CONTROLLER_REGISTERIMPL(HelpController)

bool HelpController::Construct()
{
    return true;
}

bool HelpController::Validate(const SessionId& sessionId, const ControllerArguments& arguments)
{
    return true;
}

CONTROLLER_ACTIONVALIDATEIMPL(HelpController, ShowControllers, "ShowControllers", "Print all controller with their actions.")
bool HelpController::ShowControllersValidate(const SessionId& sessionId, const ControllerArguments& arguments)
{
    return true;
}

void HelpController::ShowControllers(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& outContent)
{
    enum OutputFormat
    {
        OF_Text,
        OF_Html,
        OF_Json,
        OF_Xml
    };

    OutputFormat outputFormat = OutputFormat::OF_Text;
    auto it = arguments.find("format");
    if(it != arguments.end())
    {
        const string formatText = it->second;

        if(formatText == "text")
            outputFormat = OutputFormat::OF_Text;
        else if(formatText == "html")
            outputFormat = OutputFormat::OF_Html;
        else if(formatText == "json")
            outputFormat = OutputFormat::OF_Json;
        else if(formatText == "xml")
            outputFormat = OutputFormat::OF_Xml;
    }

    switch (outputFormat)
    {
    case OF_Text:
        ShowControllersText(sessionId, arguments, outContent);
        break;
    case OF_Html:
        ShowControllersHTML(sessionId, arguments, outContent);
        break;
    case OF_Json:
        ShowControllersJSON(sessionId, arguments, outContent);
        break;
    case OF_Xml:
        ShowControllersXML(sessionId, arguments, outContent);
        break;
    }
}

void HelpController::ShowControllersText(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& outContent)
{
    outContent.append("Controllers:\n\n");

    for(auto& controllerPair : ControllerManager::GetControllers())
    {
        outContent.append("\t" +
                       controllerPair.second->GetName() + " - " +
                       controllerPair.second->GetDescription() +
                       "\n");

        for(auto& actionPair : controllerPair.second->GetActions())
        {
            outContent.append("\t\t" +
                           actionPair.second->GetName() + " - " +
                           actionPair.second->GetDescription() +
                           "\n");
        }
    }
}

void HelpController::ShowControllersHTML(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& outContent)
{
#ifdef WITH_CTPP
    outContent.append("Format html don't sapport\n");
#else
    outContent.append("Format html don't sapport\n");
#endif
}

void HelpController::ShowControllersJSON(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& outContent)
{
#ifdef WITH_JSONCPP
    Json::Value controllersValue;

    for(auto& controllerPair : ControllerManager::GetControllers())
    {
        Json::Value controllerValue;

        Json::Value actionsValue(Json::arrayValue);

        for(auto& actionPair : controllerPair.second->GetActions())
        {
            Json::Value actionValue;
            actionValue["name"] =  actionPair.second->GetName();
            actionValue["description"] =  actionPair.second->GetDescription();

            actionsValue.append(actionValue);
        }

        controllerValue["name"] = controllerPair.second->GetName();
        controllerValue["description"] = controllerPair.second->GetDescription();
        controllerValue["actions"] = actionsValue;

        controllersValue[controllerPair.second->GetName()] = controllerValue;
        //controllersValue.append(controllerValue);
    }

    const std::string jsonText = controllersValue.toStyledString();
    outContent.append(jsonText);
#else
    outContent.append("Format json don't sapport\n");
#endif
}

void HelpController::ShowControllersXML(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& outContent)
{
#ifdef WITH_TINYXML2
    outContent.append("Format xml don't sapport\n");
#else
    outContent.append("Format xml don't sapport\n");
#endif
}

} // End http.
