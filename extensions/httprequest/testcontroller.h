#ifndef TESTCONTROLLER_H
#define TESTCONTROLLER_H

#ifdef WITH_TESTCONTROLLER
#include <controller.h>

namespace openrc {

class HttpTestController : public IController
{
    CONTROLLER_REGISTER(HttpTestController, "HttpTest", "Http test controller.")

public:
    CONTROLLER_ACTION(HttpTestController, DoAction)
};

}
#endif

#endif // EXAMPLECONTROLLER_H
