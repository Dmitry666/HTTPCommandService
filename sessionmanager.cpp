#include "sessionmanager.h"


#ifndef _MSC_VER
// save diagnostic state
#pragma GCC diagnostic push
// turn off the specific warning. Can also use "-Wall"
#pragma GCC diagnostic ignored "-Wall"
//#pragma GCC diagnostic ignored "-Wunused-variable"
//#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#define BOOST_CONFIG_WARNING_DISABLE_HPP
#pragma GCC system_header
#endif

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <mutex>

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

namespace openrc {

std::map<SessionKey, SessionShared> _sessions;
int32 _lastSessionId = -1;
std::mutex _mutex;

SessionWeak SessionManager::NewSession()
{
	std::lock_guard<std::mutex> lock(_mutex);

    SessionId sessionId;
    sessionId.Id = ++_lastSessionId;

    boost::uuids::uuid tag = boost::uuids::random_generator()();
    sessionId.Key = boost::lexical_cast<std::string>(tag);

	auto session = std::make_shared<Session>(sessionId);
    _sessions.insert(std::make_pair(sessionId.Key, session));
  
	return session;
}

SessionWeak SessionManager::FindSessionByKey(const SessionKey& key)
{
	{
		std::lock_guard<std::mutex> lock(_mutex);

		auto it = _sessions.find(key);
		if(it != _sessions.end())
		{
			return it->second;
		}
	}

    return NewSession();
}

} // End http.
