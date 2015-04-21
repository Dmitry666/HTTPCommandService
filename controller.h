#ifndef HTTP_CONTROLLER_H
#define HTTP_CONTROLLER_H

#include <common.h>

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
    virtual const char* ClassName() { return getClassNameStatic(); } \
    virtual void RegisterMethods(); \
    static const char* GetClassNameStatic() { return textName; } \
private:

#define FACTORY_REGISTERIMPL(classname, collection) \
    const TControllerRegistrar<classname, ControllerManager> classname::creator##classname;

/**
 * @brief Controller interface.
 */
class IController
{
public:
    virtual const char* ClassName() = 0;
    virtual void RegisterMethods() = 0;
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

}

#endif // CONTROLLER

