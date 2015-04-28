#include "javascriptcontroller.h"
#include "javascriptmanager.h"

#ifdef WITH_JAVASCRIPT

using namespace std;

namespace http {

bool JavaScriptControllerMethod::Validate(class IController* obj,
                        const SessionId& sessionId,
                        const ControllerArguments& arguments) const
{
    return true;
}

void JavaScriptControllerMethod::Execute(class IController* obj,
                        const SessionId& sessionId,
                        const ControllerArguments& arguments,
                        ControllerOutput& outContent)
{
    JavascriptController* class_ = static_cast<JavascriptController*>(obj);

	map<string, string> options;
	map<string, string> output;

	for(auto& pair : arguments)
	{
		options.insert(std::make_pair(pair.first, pair.second));
	}

	int32 result = JavascriptManager::Instance().Execute(class_->GetName() + ".js", options, output);

	outContent.append( "Result: " + std::to_string(result) + "\n" );

	for(auto& pair : output)
	{
		outContent.append( "\t" + pair.first + ": " + pair.second + "\n" );
	}
}

bool JavascriptController::Construct()
{
    return true;
}

bool JavascriptController::ActionExecute(
            const std::string& actionName,
            const SessionId& sessionId,
            const ControllerArguments& arguments,
            ControllerOutput& outContent)
{
	map<string, string> options;
	map<string, string> output;

	int32 result = JavascriptManager::Instance().Execute(GetName() + ".js", options, output);
	outContent.append( "Result: " + std::to_string(result) );
    return true;
}

} // End http.

#endif
