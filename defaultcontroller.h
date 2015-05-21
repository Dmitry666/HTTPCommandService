#ifndef HTTP_DEFAULTCONTROLLER_H
#define HTTP_DEFAULTCONTROLLER_H

#include "controller.h"

namespace openrc {

/**
 * @brief Test default controller.
 */
class DefaultController : public IController
{
    CONTROLLERVALIDATE_REGISTER(DefaultController, "DefaultTest", "Controller from testing.")

public:
    CONTROLLER_ACTIONVALIDATE(DefaultController, Test)
};

} // End http.

#endif // DEFAULTCONTROLLER_H
