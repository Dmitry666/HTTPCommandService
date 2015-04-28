#ifndef HTTP_JAVASCRIPTCONTROLLER_H
#define HTTP_JAVASCRIPTCONTROLLER_H

#ifdef WITH_JAVASCRIPT

#include "controller.h"

namespace http {

	/**
 * @brief Template controller action.
 */

class JavaScriptControllerMethod : public ControllerMethod
{
public:
public:
    JavaScriptControllerMethod(const std::string& name,
                      const std::string& description)
        : ControllerMethod(name, description)
    {}

    virtual bool IsValidateMethod() const override
    {
		return false; // _validate ? true : false;
    }

	virtual bool Validate(class IController* obj,
		const SessionId& sessionId,
		const ControllerArguments& arguments) const override;

	virtual void Execute(class IController* obj,
		const SessionId& sessionId,
		const ControllerArguments& arguments,
		ControllerOutput& contents) override;
};

class JavascriptController : public IController
{
public:
    JavascriptController(const std::string& name, const std::string& description)
        : IController(name, description)
    {}

    virtual ~JavascriptController()
    {}

	virtual bool Construct() override;
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

    void JSRegisterMethod(const std::string& name, const std::string& description)
    {
        RegisterMethod(new JavaScriptControllerMethod(name, description));
    }
};

} // End http.

#endif

#endif // JAVASCRIPTCONTROLLER_H
