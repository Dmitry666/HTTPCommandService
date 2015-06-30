#include "dbcontext.h"

using namespace std;

#ifdef WITH_JAVASCRIPT
#include <v8pp/class.hpp>
#include <v8pp/module.hpp>

using namespace v8;

DBContext::DBContext( const v8::FunctionCallbackInfo<v8::Value>& args )
{
	//value_ = v8pp::from_v8<double>(args.GetIsolate(), args[0], 0);
}
#else
DBContext::DBContext( )
{}
#endif

DBContext::~DBContext()
{}

bool DBContext::Open()
{
	std::string connectLine;
	try
	{
		if(_type == "postgresql")
		{
			connectLine = "dbname=" + _dbName + " user=" + _user + " password=" + _password;
			_sql.open(soci::postgresql, connectLine.c_str());
		}
#if 0
		else if(_type == "mysql")
		{
			connectLine = "host=" + host + " dbname=" + dbname + " user=" + user + " password=" + password;
			_sql.open(soci::mysql, connectLine.c_str());
		}
#endif
		else
		{
			return false;
		}

		_isConnected = true;
	}
	catch (std::exception &e)
	{
		// e.what()
		_isConnected = false;
	}

	return _isConnected;
}

bool DBContext::Close()
{
	try
	{
		_sql.close();
	}
	catch (std::exception &e)
	{
		//ErrorStream::Write("[Net]: Data base. Exception %s", e.what());
	}

	return true;
}

soci::rowset<soci::row> DBContext::Query(const std::string& text)
{
	try
	{
		soci::rowset<soci::row> rowset = _sql.prepare << text;
		return rowset;
	}
	catch (std::exception &e)
	{
		throw;
	}

	//return soci::rowset<soci::row>();
}

#ifdef WITH_JAVASCRIPT
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
#endif


DBContext& DBContext::Instance(const string& name)
{
    static std::map<string, DBContext*> contexts;
    auto it = contexts.find(name);
    if(it != contexts.end())
    {
        DBContext* context = new DBContext();
        contexts.insert(std::make_pair(name, context));
        return *context;
        //context->SetAddress("176.120.25.177");
        //context->SetDBName("instagram_tokens");
    }

    return *it->second;
}
