#ifndef HTTP_MODULEMANAGER_H
#define HTTP_MODULEMANAGER_H

#include "module.h"

namespace http {

class ModuleManager
{
    typedef std::map<std::string, IModule*> ModuleMap;

public:
    ModuleManager();
    ~ModuleManager();

    void RegisterModule(const std::string& moduleName, IModule* module);
    void InitializeAll();
    void ShutdownAll();

    void LoadModuleFromFile(const std::string& filepath);
    void LoadModulesFromFolder(const std::string& folderPath);

    static ModuleManager& Instance();

    template<typename TModule>
    static TModule& RegisterModule( const std::string& moduleName )
    {
        TModule* module = new TModule();
        ModuleManager& moduleManager = ModuleManager::Instance();
        ModuleManager::RegisterModule(moduleName, module);
        return *module;
    }

protected:
    ModuleMap _modules;

};

} // End http.

#endif // MODULEMANAGER_H
