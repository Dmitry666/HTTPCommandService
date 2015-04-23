#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include "controller.h"

namespace http {

class SessionManager
{
public:
    SessionManager()
    {}

    ~SessionManager()
    {}

    static SessionId NewSession();
    static SessionId FindSessionByKey(const SessionKey& key);

protected:

};

} // End http.

#endif // SESSIONMANAGER_H
