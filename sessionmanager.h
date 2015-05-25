#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include "common.h"
#include <memory>
namespace openrc {

//typedef std::string ControllerOutput;

typedef std::string SessionKey;

/**
 * @brief Session identificator struct.
 */
struct SessionId
{
    int32 Id;
    SessionKey Key;

    SessionId()
        : Id(-1)
    {}

    SessionId(const SessionId& sessionId)
        : Id(sessionId.Id)
        , Key(sessionId.Key)
    {}

    bool IsValid() const
    {
        return Id != -1;
    }

    SessionId& operator = (const SessionId& sessionId)
    {
        Id = sessionId.Id;
        Key = sessionId.Key;
        return (*this);
    }
};

class Session
{
public:
	Session()
	{}

	Session(const SessionId& id)
		: Id(id)
	{}

	~Session()
	{}

public:
	SessionId Id;


};

typedef std::shared_ptr<Session> SessionShared;
typedef std::weak_ptr<Session> SessionWeak;

class SessionManager
{
public:
    SessionManager()
    {}

    ~SessionManager()
    {}

    static SessionWeak NewSession();
    static SessionWeak FindSessionByKey(const SessionKey& key);

protected:

};

} // End http.

#endif // SESSIONMANAGER_H
