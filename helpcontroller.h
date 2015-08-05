#ifndef HTTP_HELPCONTROLLER_H
#define HTTP_HELPCONTROLLER_H


#include "controller.h"

namespace openrc {

/**
 * @brief Help controller from common information.
 */
class HelpController : public IController
{
    CONTROLLERVALIDATE_REGISTER(HelpController, "Help", "Help information.")

public:
    CONTROLLER_ACTION(HelpController, ShowControllers)
    CONTROLLER_ACTION(HelpController, Stats)

private:
    void ShowControllersText(SessionWeak session, const ControllerArguments& arguments, ControllerOutput& content);
    void ShowControllersHTML(SessionWeak session, const ControllerArguments& arguments, ControllerOutput& content);
    void ShowControllersJSON(SessionWeak session, const ControllerArguments& arguments, ControllerOutput& content);
    void ShowControllersXML(SessionWeak session, const ControllerArguments& arguments, ControllerOutput& content);
};

} // End http.
#endif // HELLOCONTROLLER_H
