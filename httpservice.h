#ifndef HTTPSERVICE_H
#define HTTPSERVICE_H

#include "common.h"


namespace openrc {

/**
 * @brief Argument container from service.
 */
struct HttpServiceArguments
{
    typedef std::map<std::string, std::string> ArgumentsContainer;

    HttpServiceArguments()
    {}

    HttpServiceArguments(const HttpServiceArguments& ca)
        : _argumentMap(ca._argumentMap)
    {}

    HttpServiceArguments(const ArgumentsContainer& argumentMap)
        : _argumentMap(argumentMap)
    {}

    std::string operator [](const std::string& key) const
    {
        auto it = _argumentMap.find(key);
        if(it != _argumentMap.end())
        {
            return it->second;
        }

        return "";
    }

	void Push(const std::string& key, const std::string& value)
	{
		_argumentMap.insert(std::make_pair(key, value));
	}

	const std::map<std::string, std::string>& ToArgumentMap() const { return _argumentMap; }

    // From stl.
    ArgumentsContainer::iterator begin() {return _argumentMap.begin();}
    ArgumentsContainer::const_iterator begin() const {return _argumentMap.begin();}
    ArgumentsContainer::iterator end() {return _argumentMap.end();}
    ArgumentsContainer::const_iterator end() const {return _argumentMap.end();}
    ArgumentsContainer::iterator find(const std::string& key) {return _argumentMap.find(key);}
    ArgumentsContainer::const_iterator find(const std::string& key) const {return _argumentMap.find(key);}

private:
    ArgumentsContainer _argumentMap;
};

/**
 * @brief Http service manager.
 */
class HttpService
{
public:
    HCORE_API HttpService(const HttpServiceArguments& arguments);
    HCORE_API ~HttpService();

	/**
	 * @brief Start service by address and port.
	 * @param address
	 * @param port server port.
	 * @return started.
	 */
    HCORE_API bool Start(const std::string& address, const std::string& port);

	/**
	 * @brief Stop service.
	 * @return stoped.
	 */
    HCORE_API bool Stop();

	/**
	 * @brief Wait stoped service.
	 * @return
	 */
    HCORE_API bool Join(float time = 0.f);

private:
    //void Run();

private:
	HttpServiceArguments _arguments;
    std::string _address;
    std::string _port;
};

} // End http.

#endif // HTTPSERVICE_H
