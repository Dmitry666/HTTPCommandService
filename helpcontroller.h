#ifndef HTTP_HELPCONTROLLER_H
#define HTTP_HELPCONTROLLER_H


#include "controller.h"

namespace http {

/**
 * @brief Help controller from common information.
 */
class HelpController : public IController
{
    CONTROLLERVALIDATE_REGISTER(HelpController, "Help", "Help information.")

public:
    CONTROLLER_ACTIONVALIDATE(HelpController, ShowControllers)

private:
    void ShowControllersText(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& content);
    void ShowControllersHTML(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& content);
    void ShowControllersJSON(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& content);
    void ShowControllersXML(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& content);
};

} // End http.
#endif // HELLOCONTROLLER_H
