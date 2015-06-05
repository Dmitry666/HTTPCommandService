#ifndef EXAMPLECONTROLLER_H
#define EXAMPLECONTROLLER_H

#include <controller.h>

namespace openrc {

class AsyncController : public IController
{
    CONTROLLER_REGISTER(AsyncController, "Async", "Async command.")

public:
    CONTROLLER_ACTION(AsyncController, Help)
    CONTROLLER_ACTION(AsyncController, AsyncTest)
	CONTROLLER_ACTION(AsyncController, ThreadTest)
};

}

#endif // EXAMPLECONTROLLER_H
