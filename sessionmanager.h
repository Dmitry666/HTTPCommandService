#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include "common.h"
#include <memory>
#include <mutex>

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

/**
 * @brief Network session.
 */
class Session
{
public:
	struct AsyncCommand
	{
		std::string Controller;
		std::string Method;
		std::string Body;

		AsyncCommand()
		{}

		AsyncCommand(const AsyncCommand& command)
			: Controller(command.Controller)
			, Method(command.Method)
			, Body(command.Body)
		{}

		AsyncCommand(AsyncCommand&& command)
		{
            Controller = std::move(command.Controller);
            Method = std::move(command.Method);
            Body = std::move(command.Body);
		}
	};

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
	std::vector<AsyncCommand> Commands;
	//struct 

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
};

} // End http.

#endif // SESSIONMANAGER_H
