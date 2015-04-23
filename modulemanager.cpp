#include "modulemanager.h"
#include "directoryinfo.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace http {

ModuleManager :: ModuleManager()
{}

ModuleManager :: ~ModuleManager()
{}

void ModuleManager :: RegisterModule(const std::string& moduleName, IModule* module)
{
    _modules.insert(std::make_pair(moduleName, module));
}

void ModuleManager :: InitializeAll()
{
    for (auto& pair : _modules)
    {
        pair.second->InitialiseModule();
    }
}

void ModuleManager :: ShutdownAll()
{
    for (auto& pair : _modules)
    {
        pair.second->ShutdownModule();
    }
}

void ModuleManager::LoadModuleFromFile(const std::string& filepath)
{
#ifdef _WIN32
    HMODULE module = LoadLibraryA(filepath.c_str());
#else
    void* module = dlopen(filepath.c_str(), RTLD_LAZY);
#endif

    typedef IModule* (*ModuleGeter)();

    if (module)
    {
#ifdef _WIN32
        ModuleGeter getFunction = (ModuleGeter)GetProcAddress(module, "GetModule");
#else
        ModuleGeter getFunction = (ModuleGeter)dlsym(module, "GetModule");
#endif

        if (getFunction)
        {
            IModule* moduleInstance = getFunction();
            if (moduleInstance != nullptr)
            {
                RegisterModule(moduleInstance->GetModuleName(), moduleInstance);
            }
        }

    }
}

void ModuleManager::LoadModulesFromFolder(const std::string& folderPath)
{
    auto files = DirectoryInfo::GetFiles(folderPath);
    for (auto& file : files)
    {
        if (file.Extenstion() == "dll" || file.Extenstion() == "so")
        {
            const std::string& path = file.GetFullPath();
            LoadModuleFromFile(path);
        }
    }
}

ModuleManager& ModuleManager::Instance()
{
    static ModuleManager moduleManager;
    return moduleManager;
}

}
 // End http.
