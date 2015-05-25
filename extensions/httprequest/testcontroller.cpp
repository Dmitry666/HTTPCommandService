#include "testcontroller.h"
#include "httprequest.h"

#ifdef WITH_TESTCONTROLLER

using namespace std;
namespace openrc {

CONTROLLER_REGISTERIMPL(HttpTestController)
bool HttpTestController::Construct()
{
    return true;
}

CONTROLLER_ACTIONIMPL(HttpTestController, DoAction, "DoAction", "Request http.")
void HttpTestController::DoAction(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& outContent)
{
	const std::string url = arguments["url"];

	XMLHttpRequest request;
	request.Open("GET", url, false);
	request.OnReadyStateChange = [&outContent](const std::string& responseText){
		outContent.Append(responseText);
	};
	request.Send();
}

}

#endif