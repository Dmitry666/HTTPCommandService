#ifndef EXAMPLECONTROLLER_H
#define EXAMPLECONTROLLER_H

#include <controller.h>

namespace http {

class ExampleController : public IController
{
    CONTROLLERVALIDATE_REGISTER(ExampleController, "ExampleCpp", "Example cpp module.")

public:
    CONTROLLER_ACTIONVALIDATE(ExampleController, DoAction)
};

}

#endif // EXAMPLECONTROLLER_H
