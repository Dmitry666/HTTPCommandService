#ifndef HTTP_COMMON_H
#define HTTP_COMMON_H

#include <stdio.h>
//#include <string>
#include <map>
#include <functional>
#include <vector>

#ifdef _MSC_VER
#include <stdint.h>
#include <algorithm>
#include <iostream>       // std::cout
#include <string>
#else
#include <string.h>
#endif

typedef int32_t int32;

#ifdef _MSC_VER
#ifdef __COMPILING_HCORE
#define HCORE_API __declspec(dllexport)
#else
#define HCORE_API __declspec(dllimport)
#endif
#else
#define HCORE_API
#endif

#endif // COMMON

