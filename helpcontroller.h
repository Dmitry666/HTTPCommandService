#ifndef HTTP_HELPCONTROLLER_H
#define HTTP_HELPCONTROLLER_H


#include "controller.h"

namespace http {

/**
 * @brief Help controller from common information.
 */
class HelpController : public IController
{
    CONTROLLER_REGISTER(HelpController, "Help", "Help information.")

public:
    CONTROLLER_ACTION(HelpController, ShowControllers)
};

} // End http.
#endif // HELLOCONTROLLER_H
