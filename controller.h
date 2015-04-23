#ifndef HTTP_CONTROLLER_H
#define HTTP_CONTROLLER_H

#include "common.h"

namespace http {

typedef std::map<std::string, std::string> ControllerArguments;
typedef std::string ControllerOutput;

struct SessionId
{
    int32 Id;
};

#define CONTROLLER_REGISTER(classname, textName, description) \
private: \
    static const TControllerRegistrar<classname, ControllerManager> creator##classname; \
public: \
    classname() \
        : IController(textName, description) \
    {} \
    \
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
    virtual const char* ClassName() { return GetClassNameStatic(); } \
    virtual bool Validate(const SessionId& sessionId, const ControllerArguments& arguments) const override;\
    /*virtual void RegisterMethods();*/ \
    static const char* GetClassNameStatic() { return textName; } \
private:


#define CONTROLLER_REGISTERIMPL(classname) \
    const TControllerRegistrar<classname, ControllerManager> classname::creator##classname;

#define CONTROLLER_ACTION(classname, methodname) \
    void methodname(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& outContents); \
    static const TMethodRegistrar<classname> creator##classname##methodname;

#define CONTROLLER_ACTIONIMPL(classname, methodname, actionname, description) \
    const TMethodRegistrar<classname> classname::creator##classname##methodname(actionname, description, &classname::methodname);



#define CONTROLLER_ACTIONVALIDATE(classname, methodname) \
    void methodname(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& outContents); \
    bool methodname##Validate(const SessionId& sessionId, const ControllerArguments& arguments) const; \
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
     * @brief Validate method from session.
     * @param sessionId session identificator.
     * @return validation success.
     */
    virtual bool Validate(class IController* obj,
                          const SessionId& sessionId,
                          const ControllerArguments& arguments) const = 0;

    /**
     * @brief Execute this controller action.
     * @param obj controller pointer.
     * @param arguments request arguments.
     * @param contents out content.
     */
    virtual void Execute(class IController* obj,
                         const SessionId& sessionId,
                         const ControllerArguments& arguments,
                         ControllerOutput& contents) = 0;

    /**
     * @brief Execute operator.
     * @param obj controller pointer.
     * @param arguments request arguments.
     * @param contents out content.
     * @return controller method reference.
     */
    ControllerMethod& operator ()(class IController* obj,
                                  const SessionId& sessionId,
                                  const ControllerArguments& arguments,
                                  ControllerOutput& contents)
    {
        this->Execute(obj, sessionId, arguments, contents);
        return (*this);
    }

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
    typedef std::function<void(ClassType&, const SessionId&, const ControllerArguments&, ControllerOutput& outContent)> FunctionType;
    typedef std::function<bool(const ClassType&, const SessionId&, const ControllerArguments&)> ValidateType;

public:
    TControllerMethod(const std::string& name,
                      const std::string& description,
                      FunctionType function,
                      ValidateType validate)
        : ControllerMethod(name, description)
        , _function(function)
        , _validate(validate)
    {}

    virtual bool Validate(class IController* obj,
                          const SessionId& sessionId,
                          const ControllerArguments& arguments) const override
    {
        if(_validate)
        {
            ClassType* class_ = static_cast<ClassType*>(obj);
            return _validate(*class_, sessionId, arguments);
        }

        return true;
    }

    virtual void Execute(class IController* obj,
                         const SessionId& sessionId,
                         const ControllerArguments& arguments,
                         ControllerOutput& contents) override
    {
        ClassType* class_ = static_cast<ClassType*>(obj);
        _function(*class_, sessionId, arguments, contents);
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
    IController(const std::string& name, const std::string& description)
        : _name(name)
        , _description(description)
    {}

    virtual const char* ClassName() = 0;

    /**
     * @brief Validate controller from session.
     * @param sessionId session identificator.
     * @return validation success.
     */
    virtual bool Validate(const SessionId& sessionId, const ControllerArguments& arguments) const
    {
        return true;
    }

    /**
     * @brief Find method
     * @param name method name.
     * @return method.
     */
    virtual ControllerMethodRef FindMethod(const std::string& name);

    /**
     * @brief Register new method.
     * @param name method name.
     * @param method method.
     */
    void RegisterMethod(ControllerMethodRef method);

    template<typename ClassType>
    void TRegisterMethod(const std::string& name,
                         const std::string& description,
                         void(ClassType:: *method)(const SessionId&, const ControllerArguments&, ControllerOutput& outContent),
                         bool(ClassType:: *validate)(const SessionId&, const ControllerArguments&)const = nullptr)
    {
        typename TControllerMethod<ClassType>::FunctionType actionFunc = method;
        typename TControllerMethod<ClassType>::ValidateType validateFunc = validate;
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
            const SessionId& sessionId,
            const ControllerArguments& arguments,
            ControllerOutput& outContent)
    {
        auto methodRef = FindMethod(actionName);
        if( methodRef == nullptr )
        {
            return false;
        }

        (*methodRef)(this, sessionId, arguments, outContent);
        return true;
    }

    //
    const std::string& GetName() const {return _name;}
    const std::string& GetDescription() const {return _description;}
    const std::map<std::string, ControllerMethodRef>& GetActions() const {return _methods;}

protected:
    std::string _name;
    std::string _description;
    std::map<std::string, ControllerMethodRef> _methods;
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
    static void RegisterController(IController* controller);

    /**
     * @brief Find controller by name.
     * @param name controller name.
     * @return controller pointer.
     */
    static IController* FindController(const std::string& name);

    /**
     * @brief Get all controllers.
     * @return Controllers map.
     */
    static const std::map<std::string, IController*>& GetControllers() {return _controllers;}

private:
    //static ControllerManager& Instance();
    static std::map<std::string, IController*> _controllers;
};

template<typename Type, typename Collection>
class TControllerRegistrar
{
public:
    TControllerRegistrar()
    {
        Collection::RegisterController(new Type() );
    }
};

template<typename ClassType>
class TMethodRegistrar
{
public:
    TMethodRegistrar(const std::string& name,
                     const std::string& description,
                     void(ClassType:: *function)(const SessionId&, const ControllerArguments&, ControllerOutput&),
                     bool(ClassType:: *validate)(const SessionId&, const ControllerArguments&)const = nullptr
                     )
    {
        IController* controller = ControllerManager::FindController(ClassType::GetClassNameStatic());
        controller->TRegisterMethod(name, description, function, validate);
    }
};

} // End http.

#endif // CONTROLLER

