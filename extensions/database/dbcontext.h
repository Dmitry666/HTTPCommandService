#ifndef DBCONTEXT_H
#define DBCONTEXT_H

#include <module.h>

#include <include/v8.h>
#include <include/libplatform/libplatform.h>

#include <map>
#include <string>

class QueryResult
{
};

class DBContext
{
public:
	explicit DBContext( const v8::FunctionCallbackInfo<v8::Value>& args );
	virtual ~DBContext();

	bool Open();
	bool Close();

	QueryResult Query(const std::string& text);

	void SetType(const std::string& type) { _type = type; }
	const std::string& GetType() const { return _type; }

	void SetAddress(const std::string& address) { _address = address; }
	const std::string& GetAddress() const { return _address; }

	void SetPort(const std::string& port) { _port = port; }
	const std::string& GetPort() const { return _port; }

	void SetDBName(const std::string& dbName) { _dbName = dbName; }
	const std::string& GetDBName() const { return _dbName; }

	void SetUser(const std::string& user) { _user = user; }
	const std::string& GetUser() const { return _user; }

	void SetPassword(const std::string& password) { _password = password; }
	const std::string& GetPassword() const { return _password; }

	static void Register(v8::Isolate* isolate);

protected:
	std::string _type;
	std::string _address;
	std::string _port;
	std::string _dbName;
	std::string _user;
	std::string _password;
};

#endif // EXAMPLECPPMODULE_H