#ifndef COMMONPRIVATE
#define COMMONPRIVATE

#include "common.h"

#ifdef _MSC_VER
//#define _WIN32_WINNT 0x0501
#endif

namespace openrc {

std::vector<std::string> split(const std::string& str, char delimiter);
std::string quoted(const std::string& text);

}

#endif // COMMONPRIVATE

