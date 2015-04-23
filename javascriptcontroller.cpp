#include "javascriptcontroller.h"

#ifdef WITH_JAVASCRIPT

namespace http {

bool JavascriptController::ActionExecute(
            const std::string& actionName,
            const SessionId& sessionId,
            const ControllerArguments& arguments,
            ControllerOutput& outContent)
{


    return true;
}

} // End http.

#endif
