#include "testcontroller.h"
#include "dbcontext.h"

#ifdef WITH_TESTCONTROLLER

using namespace std;
namespace openrc {

CONTROLLER_REGISTERIMPL(DBTestController)
bool DBTestController::Construct()
{
    return true;
}

CONTROLLER_ACTIONIMPL(DBTestController, DoAction, "DoAction", "Request db.")
void DBTestController::DoAction(const SessionId& sessionId, const ControllerArguments& arguments, ControllerOutput& outContent)
{
	DBContext context;
	context.SetType(arguments["type"]);
	context.SetAddress(arguments["address"]);
	context.SetPort(arguments["port"]);
	context.SetDBName(arguments["dbname"]);
	context.SetUser(arguments["user"]);
	context.SetPassword(arguments["password"]);

	context.Open();
	auto rowset = context.Query(arguments["sql"]);
	outContent.Append(rowset.begin() != rowset.end() ? "OK" : "Error");
	context.Close();
}

}

#endif