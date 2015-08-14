#include "helpcontroller.h"

#ifdef WITH_JSONCPP
#include <json/writer.h>
#endif

#include "serverstats.h"

using namespace std;

namespace openrc {

CONTROLLER_REGISTERIMPL(HelpController)

bool HelpController::Construct()
{
    return true;
}

bool HelpController::Validate(SessionWeak session, const ControllerArguments& arguments)
{
    UNUSED(session)
    UNUSED(arguments)

    return true;
}

CONTROLLER_ACTIONIMPL(HelpController, ShowControllers, "ShowControllers", "Print all controller with their actions.")
void HelpController::ShowControllers(SessionWeak session, const ControllerArguments& arguments, ControllerOutput& outContent)
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
        ShowControllersText(session, arguments, outContent);
        break;
    case OF_Html:
        ShowControllersHTML(session, arguments, outContent);
        break;
    case OF_Json:
        ShowControllersJSON(session, arguments, outContent);
        break;
    case OF_Xml:
        ShowControllersXML(session, arguments, outContent);
        break;
    }
}

void HelpController::ShowControllersText(SessionWeak session, const ControllerArguments& arguments, ControllerOutput& outContent)
{
    //UNUSED(sessionId)
    //UNUSED(arguments)

    outContent.append("Controllers:\n\n");

    for(auto& controllerPair : ControllerManager::GetControllers())
    {
		IController* controller = controllerPair.second;
		if(controller->IsEnable())
		{
			outContent.append("\t" +
						   controller->GetName() + " - " +
						   controller->GetDescription() +
						   "\n");

			for(auto& actionPair : controller->GetActions())
			{
				outContent.append("\t\t" +
							   actionPair.second->GetName() + " - " +
							   actionPair.second->GetDescription() +
							   "\n");
			}
		}
    }
}

void HelpController::ShowControllersHTML(SessionWeak session, const ControllerArguments& arguments, ControllerOutput& outContent)
{
    UNUSED(session)
    UNUSED(arguments)

#ifdef WITH_CTPP
    outContent.append("Format html don't sapport\n");
#else
    outContent.append("Format html don't sapport\n");
#endif
}

void HelpController::ShowControllersJSON(SessionWeak session, const ControllerArguments& arguments, ControllerOutput& outContent)
{
    UNUSED(session)
    UNUSED(arguments)

#ifdef WITH_JSONCPP
    Json::Value controllersValue;

    for(auto& controllerPair : ControllerManager::GetControllers())
    {
		IController* controller = controllerPair.second;
		if(controller->IsEnable())
		{
			Json::Value controllerValue;

			Json::Value actionsValue(Json::arrayValue);

			for(auto& actionPair : controller->GetActions())
			{
				Json::Value actionValue;
				actionValue["name"] =  actionPair.second->GetName();
				actionValue["description"] =  actionPair.second->GetDescription();

				actionsValue.append(actionValue);
			}

			controllerValue["name"] = controllerPair.second->GetName();
			controllerValue["description"] = controllerPair.second->GetDescription();
			controllerValue["actions"] = actionsValue;

			controllersValue[controller->GetName()] = controllerValue;
			//controllersValue.append(controllerValue);
		}
    }

    const std::string jsonText = controllersValue.toStyledString();
    outContent.append(jsonText);
#else
    outContent.append("Format json don't sapport\n");
#endif
}

void HelpController::ShowControllersXML(SessionWeak session, const ControllerArguments& arguments, ControllerOutput& outContent)
{
    UNUSED(session)
    UNUSED(arguments)

#ifdef WITH_TINYXML2
    outContent.append("Format xml don't sapport\n");
#else
    outContent.append("Format xml don't sapport\n");
#endif
}

//
CONTROLLER_ACTIONIMPL(HelpController, Stats, "Stats", "Print server statistic.")
void HelpController::Stats(SessionWeak session, const ControllerArguments& arguments, ControllerOutput& outContent)
{
    UNUSED(session)
    UNUSED(arguments)

#ifdef WITH_JSONCPP
    Json::Value resultValue;

    ServerStats& serverStats = ServerStats::Instance();

    resultValue["requests"]["number"] = int32(serverStats.GetNbRequest());
    resultValue["requests"]["nps"] = serverStats.GetRequestPerSecond();
    resultValue["requests"]["bytes"] = int32(serverStats.GetNbRequestBytes());
    resultValue["requests"]["bps"] = serverStats.GetRequestBytesPerSecond();

    resultValue["response"]["number"] = int32(serverStats.GetNbResponse());
    resultValue["response"]["nps"] = serverStats.GetResponsePerSecond();
    resultValue["response"]["bytes"] = int32(serverStats.GetNbResponseBytes());
    resultValue["response"]["bps"] = serverStats.GetResponseBytesPerSecond();

    outContent.append(resultValue.toStyledString());
#else
	outContent.append("Format json don't sapport\n");
#endif
}

} // End http.
