#ifndef EXAMPLECONTROLLER_H
#define EXAMPLECONTROLLER_H

#include <controller.h>

namespace openrc {

struct User
{
    std::string Login;
    std::string Password;

    User()
    {}

    User(const User& user)
        : Login(user.Login)
        , Password(user.Password)
    {}
};

class UserController : public IController
{
    CONTROLLERVALIDATE_REGISTER(UserController, "User", "Example user authorization.")

public:
    CONTROLLER_ACTIONVALIDATE(UserController, Help)
    CONTROLLER_ACTIONVALIDATE(UserController, Login)
    CONTROLLER_ACTION(UserController, DoAction)
    CONTROLLER_ACTION(UserController, Logout)

private:
    std::map<int32, User> _users;
};

}

#endif // EXAMPLECONTROLLER_H
