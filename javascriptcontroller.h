#ifndef HTTP_JAVASCRIPTCONTROLLER_H
#define HTTP_JAVASCRIPTCONTROLLER_H

#ifdef WITH_JAVASCRIPT

#include "controller.h"

#include <include/v8.h>
#include <include/libplatform/libplatform.h>
#include "js-http-request-processor.h"

namespace http {

	/**
 * @brief Template controller action.
 */

class JavaScriptControllerMethod : public ControllerMethod
{
public:
    JavaScriptControllerMethod(
		const std::string& name,
        const std::string& description,
		v8::Isolate* isolate,
		v8::Handle<v8::Function>& function,
		v8::Handle<v8::Function>& validator)
        : ControllerMethod(name, description)
		, function_(isolate, function)
		, validator_(isolate, validator)
    {}

    virtual bool IsValidateMethod() const override
    {
		return !validator_.IsEmpty();
			//!_validatorName.empty();
    }

	virtual bool Validate(class IController* obj,
		const SessionId& sessionId,
		const ControllerArguments& arguments) override;

	virtual void Execute(class IController* obj,
		const SessionId& sessionId,
		const ControllerArguments& arguments,
		ControllerOutput& contents) override;

protected:
	v8::Persistent<v8::Function> function_;
	v8::Persistent<v8::Function> validator_;	
};

class JavascriptController 
	: public IController
	, public JsHttpRequestProcessor
{
public:
    JavascriptController(const std::string& name, const std::string& description, const std::string& filename)
        : IController(name, description)
		, JsHttpRequestProcessor()
		, _filename(filename)
    {}

    virtual ~JavascriptController()
    {}

	virtual bool Construct() override;
    virtual const char* ClassName() override {return GetName().c_str();}

    /**
     * @brief Validate controller from session.
     * @param sessionId session identificator.
     * @return validation success.
     */
	virtual bool Validate(const SessionId& sessionId, const ControllerArguments& arguments) override;

	bool ValidateExecute(
            v8::Persistent<v8::Function>& validate,
            const SessionId& sessionId,
            const ControllerArguments& arguments);

    /**
     * @brief Call action by name.
     * @param actionName controller action name.
     * @param arguments action arguments.
     * @param outContent ouput data.
     * @return is action finded.
     */
    bool ActionExecute(
            v8::Persistent<v8::Function>& action,
            const SessionId& sessionId,
            const ControllerArguments& arguments,
            ControllerOutput& outContent);

    void JSRegisterMethod(
		const std::string& name, 
		const std::string& description,
		v8::Isolate* isolate,
		v8::Handle<v8::Function>& function,
		v8::Handle<v8::Function>& validator)
    {
        RegisterMethod(new JavaScriptControllerMethod(name, description, isolate, function, validator));
    }

private:
	bool Load();
	void UnLoad();
	bool IsModified();

	v8::Handle<v8::String> ReadFile(v8::Isolate* isolate, const std::string& name);

private:
	std::string _filename;
	time_t _lastModifyTime;

	v8::Persistent<v8::Function> validator_;
	std::map<std::string, v8::Persistent<v8::Function>> _validators;
	std::map<std::string, v8::Persistent<v8::Function>> _actions;

	std::map<std::string, std::string> _options;
	std::map<std::string, std::string> _argumentsMap;
	std::map<std::string, std::string> _outputs;
};

} // End http.

#endif

#endif // JAVASCRIPTCONTROLLER_H
