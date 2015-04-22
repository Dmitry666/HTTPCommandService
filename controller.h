#ifndef HTTP_CONTROLLER_H
#define HTTP_CONTROLLER_H

#include "common.h"

namespace http {

template<typename Type, typename Collection>
class TControllerRegistrar
{
public:
    TControllerRegistrar()
    {
        Collection::RegisterController(new Type() );
    }
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
    virtual void RegisterMethods(); \
    static const char* GetClassNameStatic() { return textName; } \
private:

#define CONTROLLER_REGISTERIMPL(classname) \
    const TControllerRegistrar<classname, ControllerManager> classname::creator##classname;


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
     * @brief Execute this controller action.
     * @param obj controller pointer.
     * @param arguments request arguments.
     * @param contents out content.
     */
    virtual void Execute(class IController* obj, const std::map<std::string, std::string>& arguments, std::string& contents) = 0;

    /**
     * @brief Execute operator.
     * @param obj controller pointer.
     * @param arguments request arguments.
     * @param contents out content.
     * @return controller method reference.
     */
    ControllerMethod& operator ()(class IController* obj, const std::map<std::string, std::string>& arguments, std::string& contents)
    {
        this->Execute(obj, arguments, contents);
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
template<typename ClassType, typename... Args>
class TControllerMethod : public ControllerMethod
{
    typedef std::function<void(ClassType&, Args...)> FunctionType;

public:
    TControllerMethod(const std::string& name, const std::string& description, FunctionType function)
        : ControllerMethod(name, description)
        , _function(function)
    {}

    virtual void Execute(class IController* obj, const std::map<std::string, std::string>& arguments, std::string& contents) override
    {
        ClassType* class_ = static_cast<ClassType*>(obj);
        _function(*class_, arguments, contents);
    }

private:
    FunctionType _function;
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
    virtual void RegisterMethods() = 0;

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

    template<typename ClassType, typename... Args>
    void TRegisterMethod(const std::string& name, const std::string& description, void(ClassType:: *method)(Args...) )
    {
        RegisterMethod(new TControllerMethod<ClassType, Args...>(name, description, method));
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

} // End http.

#endif // CONTROLLER

