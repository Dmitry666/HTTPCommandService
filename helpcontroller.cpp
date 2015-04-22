#include "helpcontroller.h"

using namespace std;

namespace http {

CONTROLLER_REGISTERIMPL(HelpController)
//CONTROLLER_ACTIONIMPL(DefaultController, Test)

void HelpController::RegisterMethods()
{
    TRegisterMethod("ShowControllers", "Print all controller with their actions.", &HelpController::ShowControllers);
}

void HelpController::ShowControllers(const map<string, string>& arguments, string& content)
{
    content.append("Controllers:\n\n");

    for(auto& controllerPair : ControllerManager::GetControllers())
    {
        content.append("\t" +
                       controllerPair.second->GetName() + " - " +
                       controllerPair.second->GetDescription() +
                       "\n");

        for(auto& actionPair : controllerPair.second->GetActions())
        {
            content.append("\t\t" +
                           actionPair.second->GetName() + " - " +
                           actionPair.second->GetDescription() +
                           "\n");
        }
    }
}

} // End http.
