#include "dbcontext.h"

#include <v8pp/class.hpp>
#include <v8pp/module.hpp>

using namespace v8;

DBContext::DBContext( const v8::FunctionCallbackInfo<v8::Value>& args )
{
	//value_ = v8pp::from_v8<double>(args.GetIsolate(), args[0], 0);
}

DBContext::~DBContext()
{}

bool DBContext::Open()
{
	return false;
}

bool DBContext::Close()
{
	return false;
}

void DBContext::Register(Isolate* isolate)
{
	Isolate* isolate = Isolate::GetCurrent();

	// Prepare class binding
	v8pp::class_<DBContext> MyObject_class(isolate);

	// Prototype
	MyObject_class
		.set("open", &DBContext::Open)
		.set("close", &DBContext::Open)

		.set("type", property(&DBContext::GetType, &DBContext::SetType))
		.set("address", property(&DBContext::GetAddress, &DBContext::SetAddress))
		.set("port", property(&DBContext::GetPort, &DBContext::SetPort))
		.set("dbName", property(&DBContext::GetDBName, &DBContext::SetDBName))
		.set("user", property(&DBContext::GetUser, &DBContext::SetUser))
		.set("password", property(&DBContext::GetPassword, &DBContext::SetPassword));

	v8pp::module bindings(isolate);
	bindings.set("DBContext", MyObject_class);

	static Persistent<Object> bindings_(isolate, bindings.new_instance());



#if 0
	EscapableHandleScope handle_scope(isolate);

	Local<ObjectTemplate> result = ObjectTemplate::New(isolate);
	result->SetInternalFieldCount(1);

	// Add accessors for each of the fields of the request.
	result->SetAccessor(
		String::NewFromUtf8(isolate, "type", String::kInternalizedString),
		GetType);

	// Again, return the result through the current handle scope.
	return handle_scope.Escape(result);
#endif
}


	std::string _type;
	std::string _address;
	std::string _port;
	std::string _dbName;
	std::string _user;
	std::string _password;