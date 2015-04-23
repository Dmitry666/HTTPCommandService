#include "sessionmanager.h"

namespace http {

std::map<SessionKey, SessionId> _sessions;
int32 _lastSessionId = -1;

SessionId SessionManager::NewSession()
{
    SessionId sessionId;
    sessionId.Id = ++_lastSessionId;
    sessionId.Key = std::to_string(sessionId.Id);

    _sessions.insert(std::make_pair(sessionId.Key, sessionId));
    return sessionId;
}

SessionId SessionManager::FindSessionByKey(const SessionKey& key)
{
    auto it = _sessions.find(key);
    if(it != _sessions.end())
    {
        return it->second;
    }

    SessionId id;
    return id;
}

} // End http.
