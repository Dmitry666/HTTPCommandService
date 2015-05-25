#ifndef BASE_SERVER_H
#define BASE_SERVER_H

#include "common.h"

namespace openrc {

class base_server
{
public:
	base_server()
	{}

    base_server(const base_server&) = delete;
    base_server& operator=(const base_server&) = delete;

    virtual ~base_server()
    {}

    virtual void run() = 0;
	virtual void stop() = 0;
};

} // end http.

#endif // MODULE

