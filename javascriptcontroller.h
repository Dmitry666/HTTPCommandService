#ifndef HTTP_JAVASCRIPTCONTROLLER_H
#define HTTP_JAVASCRIPTCONTROLLER_H

#ifdef WITH_JAVASCRIPT

#include "controller.h"

namespace http {

class JavascriptController : public IController
{
public:
    JavascriptController(const std::string& name, const std::string& description)
        : IController(name, description)
    {}

    virtual ~JavascriptController()
    {}

    virtual const char* ClassName() override {return GetName().c_str();}

    /**
     * @brief Call action by name.
     * @param actionName controller action name.
     * @param arguments action arguments.
     * @param outContent ouput data.
     * @return is action finded.
     */
    virtual bool ActionExecute(
            const std::string& actionName,
            const SessionId& sessionId,
            const ControllerArguments& arguments,
            ControllerOutput& outContent) override;
};

} // End http.

#endif

#endif // JAVASCRIPTCONTROLLER_H
