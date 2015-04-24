#ifndef EXAMPLECONTROLLER_H
#define EXAMPLECONTROLLER_H

#include <controller.h>

namespace http {


class UserController : public IController
{
    CONTROLLERVALIDATE_REGISTER(UserController, "User", "Example user authorization.")

public:
    CONTROLLER_ACTIONVALIDATE(UserController, Help)
    CONTROLLER_ACTIONVALIDATE(UserController, Login)
    CONTROLLER_ACTION(UserController, DoAction)
    CONTROLLER_ACTION(UserController, Logout)

private:
    bool _authorizated;
};

}

#endif // EXAMPLECONTROLLER_H
