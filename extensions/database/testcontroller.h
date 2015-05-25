#ifndef TESTCONTROLLER_H
#define TESTCONTROLLER_H

#ifdef WITH_TESTCONTROLLER
#include <controller.h>

namespace openrc {

class DBTestController : public IController
{
    CONTROLLER_REGISTER(DBTestController, "DBTest", "Database test controller.")

public:
    CONTROLLER_ACTION(DBTestController, DoAction)
};

}
#endif

#endif // EXAMPLECONTROLLER_H
