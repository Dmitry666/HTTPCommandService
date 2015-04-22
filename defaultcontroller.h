#ifndef HTTP_DEFAULTCONTROLLER_H
#define HTTP_DEFAULTCONTROLLER_H

#include "controller.h"

namespace http {

class DefaultController : public IController
{
    CONTROLLER_REGISTER(DefaultController, "DeafaultTest", "Controller from testing.")

public:
    //CONTROLLER_ACTION(Test);

    void Test(const std::map<std::string, std::string>& arguments, std::string& content);
};

} // End http.

#endif // DEFAULTCONTROLLER_H
