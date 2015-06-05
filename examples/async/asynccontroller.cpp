#include "asynccontroller.h"
#include <future>
#include <thread>
#include <chrono>

using namespace std;
namespace openrc {

CONTROLLER_REGISTERIMPL(AsyncController)
bool AsyncController::Construct()
{
    return true;
}

CONTROLLER_ACTIONIMPL(AsyncController, Help, "Help", "Help information.")
void AsyncController::Help(SessionWeak session, const ControllerArguments& arguments, ControllerOutput& content)
{
    UNUSED(session)
    UNUSED(arguments)

    content.append("User/Login?name=user&password=qwerty.\n");
}

CONTROLLER_ACTIONIMPL(AsyncController, AsyncTest, "AsyncTest", "AsyncTest.")
void AsyncController::AsyncTest(SessionWeak sessionWeak, const ControllerArguments& arguments, ControllerOutput& content)
{
	auto session = sessionWeak.lock();

	//content.async
	auto method = GetCurrentMethod();
	auto handle = std::async(std::launch::async, [this, method, session]()
	{
		this_thread::sleep_for(std::chrono::seconds(5));

		// After
		//method->GetName();
		//session->Commands = 
	});

	//content.async
}

CONTROLLER_ACTIONIMPL(AsyncController, ThreadTest, "ThreadTest", "ThreadTest.")
void AsyncController::ThreadTest(SessionWeak sessionWeak, const ControllerArguments& arguments, ControllerOutput& content)
{
	auto session = sessionWeak.lock();
	
	auto method = GetCurrentMethod();
	auto handle = std::async(std::launch::async, [this, method, session]()
	{
		this_thread::sleep_for(std::chrono::seconds(5));
		// After
	});
}

}
