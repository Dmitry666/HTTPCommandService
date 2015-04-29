#ifndef HTTP_SERVICE_CONFIG_H
#define HTTP_SERVICE_CONFIG_H

#ifdef WITH_INICONFIG
#include "common.h"

namespace http {

class ServiceConfig
{
private:
    ServiceConfig(const std::string& filename);

public:
    int ParseError();
    std::string Get(const std::string& section, const std::string& name, const std::string& default_value);

    long GetInteger(const std::string& section, const std::string& name, long default_value);
    double GetReal(const std::string& section, const std::string& name, double default_value);
    bool GetBoolean(const std::string& section, const std::string& name, bool default_value);

	static ServiceConfig& Instance();

private:
    static std::string MakeKey(const std::string& section, const std::string& name);
    static int ValueHandler(void* user, const char* section, const char* name,
                            const char* value);

private:
	int _error;
    std::map<std::string, std::string> _values;
};

} // End http.

#endif
#endif