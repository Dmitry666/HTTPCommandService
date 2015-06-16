#ifndef HTTP_CONTROLLER_H
#define HTTP_CONTROLLER_H

#include "common.h"
#include "sessionmanager.h"

namespace openrc {

//typedef std::map<std::string, std::string> ControllerArguments;

/**
 * @brief Controller map input argument.
 */
struct ControllerArguments
{
    typedef std::map<std::string, std::string> ArgumentsContainer;

    ControllerArguments()
    {}

    ControllerArguments(const ControllerArguments& ca)
        : _argumentMap(ca._argumentMap)
    {}

    ControllerArguments(const ArgumentsContainer& argumentMap)
        : _argumentMap(argumentMap)
    {}

    std::string operator [](const std::string& key) const
    {
        auto it = _argumentMap.find(key);
        if(it != _argumentMap.end())
        {
            return it->second;
        }

        return "";
    }

	const std::map<std::string, std::string>& ToArgumentMap() const { return _argumentMap; }

    // From stl.
    ArgumentsContainer::iterator begin() {return _argumentMap.begin();}
    ArgumentsContainer::const_iterator begin() const {return _argumentMap.begin();}
    ArgumentsContainer::iterator end() {return _argumentMap.end();}
    ArgumentsContainer::const_iterator end() const {return _argumentMap.end();}
    ArgumentsContainer::iterator find(const std::string& key) {return _argumentMap.find(key);}
    ArgumentsContainer::const_iterator find(const std::string& key) const {return _argumentMap.find(key);}

private:
    ArgumentsContainer _argumentMap;
};

/**
 * @brief Controller output body.
 */
struct ControllerOutput
{
	ControllerOutput()
	{}

	HCORE_API void Append(const std::string& value);

	// From stl.
	void append(const std::string& value)
	{
		Append(value);
	}

	const std::string& GetBody() const { return _body; }

private:
	std::string _body;
};


#define CONTROLLER_REGISTER(classname, textName, description) \
private: \
    static const TControllerRegistrar<classname, ControllerManager> creator##classname; \
public: \
    classname() \
        : IController(textName, description) \
    {} \
    \
    virtual bool Construct() override; \
    virtual const char* ClassName() { return GetClassNameStatic(); } \
    /*virtual void RegisterMethods();*/ \
    static const char* GetClassNameStatic() { return textName; } \
private:

#define CONTROLLERVALIDATE_REGISTER(classname, textName, description) \
private: \
    static const TControllerRegistrar<classname, ControllerManager> creator##classname; \
public: \
    classname() \
        : IController(textName, description) \
    {} \
    \
    virtual bool Construct() override; \
    virtual const char* ClassName() { return GetClassNameStatic(); } \
    virtual bool Validate(SessionWeak session, const ControllerArguments& arguments) override;\
    /*virtual void RegisterMethods();*/ \
    static const char* GetClassNameStatic() { return textName; } \
private:


#define CONTROLLER_REGISTERIMPL(classname) \
    const TControllerRegistrar<classname, ControllerManager> classname::creator##classname;

#define CONTROLLER_ACTION(classname, methodname) \
    void methodname(SessionWeak session, const ControllerArguments& arguments, ControllerOutput& outContents); \
    static const TMethodRegistrar<classname> creator##classname##methodname;

#define CONTROLLER_ACTIONIMPL(classname, methodname, actionname, description) \
    const TMethodRegistrar<classname> classname::creator##classname##methodname(actionname, description, &classname::methodname);



#define CONTROLLER_ACTIONVALIDATE(classname, methodname) \
    void methodname(SessionWeak session, const ControllerArguments& arguments, ControllerOutput& outContents); \
    bool methodname##Validate(SessionWeak session, const ControllerArguments& arguments); \
    static const TMethodRegistrar<classname> creator##classname##methodname;

#define CONTROLLER_ACTIONVALIDATEIMPL(classname, methodname, actionname, description) \
    const TMethodRegistrar<classname>  \
        classname::creator##classname##methodname \
            (actionname, description, &classname::methodname, &classname::methodname##Validate);

/*
class ControllerMethod
{
public:
    ControllerMethod();

protected:
    std::string _name;
};
*/

/**
 * @brief Controller action.
 */
class ControllerMethod
{
public:
    ControllerMethod(const std::string& name, const std::string& description)
        : _name(name)
        , _description(description)
    {}

    virtual ~ControllerMethod()
    {}

    /**
     * @brief Method contain validate expression.
     * @return success.
     */
    virtual bool IsValidateMethod() const = 0;

    /**
     * @brief Validate method from session.
     * @param sessionId session identificator.
	 * @param arguments input arguments.
     * @return validation success.
     */
    virtual bool Validate(class IController* obj,
                          SessionWeak session,
                          const ControllerArguments& arguments) = 0;

    /**
     * @brief Execute this controller action.
     * @param obj controller pointer.
	 * @param sessionId session identificator.
     * @param arguments request arguments.
     * @param contents out content.
     */
    virtual void Execute(class IController* obj,
                         SessionWeak session,
                         const ControllerArguments& arguments,
                         ControllerOutput& contents) = 0;

    /**
     * @brief Execute operator.
     * @param obj controller pointer.
	 * @param sessionId session identificator.
     * @param arguments request arguments.
     * @param contents out content.
     * @return controller method reference.
     */
	HCORE_API ControllerMethod& operator ()(class IController* obj,
		SessionWeak session,
		const ControllerArguments& arguments,
		ControllerOutput& contents);
/*
    {
        this->Execute(obj, session, arguments, contents);
        return (*this);
    }
*/
    //
    const std::string& GetName() const {return _name;}
    const std::string& GetDescription() const {return _description;}

private:
    std::string _name;
    std::string _description;
};


/**
 * @brief Template controller action.
 */
template<typename ClassType>
class TControllerMethod : public ControllerMethod
{
public:
#ifdef _MSC_VER
    typedef void(ClassType::*FunctionType)(SessionWeak session, const ControllerArguments&, ControllerOutput& outContent);
    typedef bool(ClassType::*ValidateType)(SessionWeak session, const ControllerArguments&);
#else
    typedef std::function<void(ClassType&, SessionWeak session, const ControllerArguments&, ControllerOutput& outContent)> FunctionType;
    typedef std::function<bool(ClassType&, SessionWeak session, const ControllerArguments&)> ValidateType;
#endif

public:
    TControllerMethod(const std::string& name,
                      const std::string& description,
                      FunctionType function,
                      ValidateType validate)
        : ControllerMethod(name, description)
        , _function(function)
        , _validate(validate)
    {}

    virtual bool IsValidateMethod() const override
    {
        return _validate ? true : false;
    }

    virtual bool Validate(class IController* obj,
                          SessionWeak session,
                          const ControllerArguments& arguments) override
    {
        if(_validate)
        {
            ClassType* class_ = static_cast<ClassType*>(obj);
#ifdef _MSC_VER
            return (class_->*_validate)(session, arguments);
#else
            return _validate(*class_, session, arguments);
#endif
        }

        return true;
    }

    virtual void Execute(class IController* obj,
                         SessionWeak session,
                         const ControllerArguments& arguments,
                         ControllerOutput& contents) override
    {
        ClassType* class_ = static_cast<ClassType*>(obj);

#ifdef _MSC_VER
        (class_->*_function)(session, arguments, contents);
#else
        _function(*class_, session, arguments, contents);
#endif
    }

private:
    FunctionType _function;
    ValidateType _validate;
};

typedef ControllerMethod* ControllerMethodRef;

/**
 * @brief Controller interface.
 */
class IController
{
    //typedef std::function<void(const IController&, const std::vector<std::string>&, std::string&)> ControllerMethod;

public:
	HCORE_API IController(const std::string& name, const std::string& description);
	virtual ~IController()
	{}

    virtual bool Construct() = 0;
    virtual const char* ClassName() = 0;

	virtual bool BeginAction()
	{
		return true;
	}

	virtual bool EndAction()
	{
		return true;
	}

    /**
     * @brief Validate controller from session.
     * @param sessionId session identificator.
	 * @param arguments input arguments.
     * @return validation success.
     */
    virtual bool Validate(SessionWeak session, const ControllerArguments& arguments)
    {
        UNUSED(session)
        UNUSED(arguments)
        return true;
    }

    /**
     * @brief Find method
     * @param name method name.
     * @return method.
     */
    HCORE_API virtual ControllerMethodRef FindMethod(const std::string& name);

    /**
     * @brief Register new method.
     * @param name method name.
     * @param method method.
     */
    HCORE_API void RegisterMethod(ControllerMethodRef method);

    template<typename ClassType>
    void TRegisterMethod(const std::string& name,
                         const std::string& description,
                         void(ClassType:: *method)(SessionWeak, const ControllerArguments&, ControllerOutput& outContent),
                         bool(ClassType:: *validate)(SessionWeak, const ControllerArguments&) = nullptr)
    {
        typename TControllerMethod<ClassType>::FunctionType actionFunc = method;
        typename TControllerMethod<ClassType>::ValidateType validateFunc =
                validate != nullptr ? validate : typename TControllerMethod<ClassType>::ValidateType();
        RegisterMethod(new TControllerMethod<ClassType>(name, description, actionFunc, validateFunc));
    }

    /**
     * @brief Call action by name.
     * @param actionName controller action name.
     * @param arguments action arguments.
     * @param outContent ouput data.
     * @return is action finded.
     */
    virtual bool ActionExecute(
            const std::string& actionName,
            SessionWeak session,
            const ControllerArguments& arguments,
            ControllerOutput& outContent)
    {
        auto methodRef = FindMethod(actionName);
        if( methodRef == nullptr )
        {
            return false;
        }

        (*methodRef)(this, session, arguments, outContent);
        return true;
    }

    //
    const std::string& GetName() const {return _name;}
    const std::string& GetDescription() const {return _description;}
    const std::map<std::string, ControllerMethodRef>& GetActions() const {return _methods;}

	bool IsEnable() const { return _enable; }

	void BindCurrentMethod(ControllerMethodRef method);
	void UnBindCurrentMethod(ControllerMethodRef method);

protected:
	ControllerMethodRef GetCurrentMethod() const { return _currentMethod; }
	std::string GetCurrentMethodName() const { return _currentMethod != nullptr ? _currentMethod->GetName() : ""; }

protected:
    std::string _name;
    std::string _description;
    std::map<std::string, ControllerMethodRef> _methods;

	//
	bool _enable;

private:
	ControllerMethodRef _currentMethod;
};

/**
 * @brief Static controller container class.
 */
class ControllerManager
{
public:
    /**
     * @brief Add new controller.
     * @param controller controller pointer.
     */
    HCORE_API static void RegisterController(IController* controller);

    /**
     * @brief Find controller by name.
     * @param name controller name.
     * @return controller pointer.
     */
    HCORE_API static IController* FindController(const std::string& name);

    /**
     * @brief Get all controllers.
     * @return Controllers map.
     */
	HCORE_API static const std::map<std::string, IController*>& GetControllers();

	HCORE_API static IController* GetController(const std::string& name);

	template<typename ClassType>
	static IController* Get()
	{
		return GetController(ClassType::GetClassNameStatic());
	}
	

private:
    //static ControllerManager& Instance();
    static std::map<std::string, IController*> _controllers;
};

/**
 * @brief Template class from registration controller.
 */
template<typename Type, typename Collection>
class TControllerRegistrar
{
public:
    TControllerRegistrar()
    {
        Collection::RegisterController(new Type() );
    }
};

/**
 * @brief Template class from registration controller method.
 */
template<typename ClassType>
class TMethodRegistrar
{
public:
    TMethodRegistrar(const std::string& name,
                     const std::string& description,
                     void(ClassType:: *function)(SessionWeak session, const ControllerArguments&, ControllerOutput&),
                     bool(ClassType:: *validate)(SessionWeak session, const ControllerArguments&) = nullptr
                     )
    {
        IController* controller = ControllerManager::Get<ClassType>();
        controller->TRegisterMethod(name, description, function, validate);
    }
};

} // End http.

#endif // CONTROLLER

