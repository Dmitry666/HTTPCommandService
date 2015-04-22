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


#define CONTROLLER_REGISTER(classname, textName) \
private: \
    static const TControllerRegistrar<classname, ControllerManager> creator##classname; \
public: \
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

class ControllerMethod
{
public:
    virtual ~ControllerMethod()
    {}

    virtual void Execute(class IController* obj, const std::map<std::string, std::string>& arguments, std::string& contents) = 0;

    ControllerMethod& operator ()(class IController* obj, const std::map<std::string, std::string>& arguments, std::string& contents)
    {
        this->Execute(obj, arguments, contents);
        return (*this);
    }
};

template<typename ClassType, typename... Args>
class TControllerMethod : public ControllerMethod
{
    typedef std::function<void(ClassType&, Args...)> FunctionType;

public:
    TControllerMethod(FunctionType function)
        : _function(function)
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
    void RegisterMethod(const std::string& name, ControllerMethodRef method);

    template<typename ClassType, typename... Args>
    void TRegisterMethod(const std::string& name, void(ClassType:: *method)(Args...) )
    {
        RegisterMethod(name, new TControllerMethod<ClassType, Args...>(method));
    }

protected:
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

private:
    //static ControllerManager& Instance();
    static std::map<std::string, IController*> _controllers;
};

} // End http.

#endif // CONTROLLER

