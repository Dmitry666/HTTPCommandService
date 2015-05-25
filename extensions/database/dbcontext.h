#ifndef DBCONTEXT_H
#define DBCONTEXT_H

#include <module.h>

#include <soci/soci.h>

#ifdef WITH_POSTGRESQL
#include <soci/postgresql/soci-postgresql.h>
#endif

#ifdef WITH_MYSQL
#include <mysql/soci-mysql.h>
#endif

#ifdef WITH_JAVASCRIPT
#include <include/v8.h>
#include <include/libplatform/libplatform.h>
#endif

#include <map>
#include <string>

class QueryResult
{
};

class DBContext
{
public:
#ifdef WITH_JAVASCRIPT
	explicit DBContext( const v8::FunctionCallbackInfo<v8::Value>& args );
#else
	DBContext();
#endif
	virtual ~DBContext();

	bool Open();
	bool Close();

	soci::rowset<soci::row> Query(const std::string& text);

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

#ifdef WITH_JAVASCRIPT
	static void Register(v8::Isolate* isolate);
#endif

protected:
	std::string _type;
	std::string _address;
	std::string _port;
	std::string _dbName;
	std::string _user;
	std::string _password;

	soci::session _sql;
	bool _isConnected;
};

#endif // EXAMPLECPPMODULE_H