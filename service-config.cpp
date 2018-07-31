// Read an INI file into easy-to-access name/value pairs.

// inih and INIReader are released under the New BSD license (see LICENSE.txt).
// Go to the project home page for more info:
//
// https://github.com/benhoyt/inih

#include <algorithm>
#include <cctype>
#include <cstdlib>

#include "service-config.h"

#ifdef _WIN32
#include <inih/ini.h>
#else
#include <ini.h>
#endif

using namespace std;

namespace openrc {

ServiceConfig::ServiceConfig(const string& filename)
{
    _error = ini_parse(filename.c_str(), ValueHandler, this);
}

int ServiceConfig::ParseError()
{
    return _error;
}

string ServiceConfig::Get(const string& section, const string& name, const string& default_value)
{
    string key = MakeKey(section, name);
    return _values.count(key) ? _values[key] : default_value;
}

long ServiceConfig::GetInteger(const string& section, const string& name, long default_value)
{
    string valstr = Get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    // This parses "1234" (decimal) and also "0x4D2" (hex)
    long n = strtol(value, &end, 0);
    return end > value ? n : default_value;
}

double ServiceConfig::GetReal(const string& section, const string& name, double default_value)
{
    string valstr = Get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    double n = strtod(value, &end);
    return end > value ? n : default_value;
}

bool ServiceConfig::GetBoolean(const string& section, const string& name, bool default_value)
{
    string valstr = Get(section, name, "");
    // Convert to lower case to make string comparisons case-insensitive
    std::transform(valstr.begin(), valstr.end(), valstr.begin(), ::tolower);
    if (valstr == "true" || valstr == "yes" || valstr == "on" || valstr == "1")
        return true;
    else if (valstr == "false" || valstr == "no" || valstr == "off" || valstr == "0")
        return false;
    else
        return default_value;
}

string ServiceConfig::MakeKey(const string& section, const string& name)
{
    string key = section + "=" + name;
    // Convert to lower case to make section/name lookups case-insensitive
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    return key;
}

int ServiceConfig::ValueHandler(void* user, const char* section, const char* name, const char* value)
{
    ServiceConfig* reader = static_cast<ServiceConfig*>(user);
    string key = MakeKey(section, name);

    if (reader->_values[key].size() > 0)
        reader->_values[key] += "\n";

    reader->_values[key] += value;
    return 1;
}


ServiceConfig& ServiceConfig::Instance()
{
	static ServiceConfig serviceConfig("./config.ini");
	return serviceConfig;
}

} // End http.
