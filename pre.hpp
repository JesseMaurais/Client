// Compiled Header

// C

#include <ciso646>
#include <cstddef>
#include <cstdint>
#include <cinttypes>
#include <cstring>
#include <csignal>
#include <csetjmp>
#include <cstdlib>
#include <cstdio>
#include <cuchar>
#include <cwchar>
#include <cerrno>
#include <cmath>
#include <ctime>

// C++

#include <initializer_list>
#include <system_error>
#include <stdexcept>
#include <utility>
#include <fstream>
#include <sstream>
#include <iostream>
#include <streambuf>
#include <charconv>
#include <locale>
#include <regex>
#include <string>
#include <iterator>
#include <vector>
#include <array>
#include <stack>
#include <queue>
#include <deque>
#include <map>
#include <set>
#include <memory>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <limits>

// Operating System

#ifdef _WIN32

#if __has_include("targetver.h")
#include "targetver.h"
#endif

#if defined(_WIN32_WINNT) && !defined(WINVER)
#define WINVER _WIN32_WINNT
#endif

#if __has_include(<SDKDDKVer.h>)
#include <SDKDDKVer.h>
#else
#define _WIN32_WINNT_NT4	  0x0400 // Windows NT 4.0
#define _WIN32_WINNT_WIN2K    0x0500 // Windows 2000
#define _WIN32_WINNT_WINXP    0x0501 // Windows XP
#define _WIN32_WINNT_WS03     0x0502 // Windows Server 2003
#define _WIN32_WINNT_VISTA    0x0600 // Windows Vista
#define _WIN32_WINNT_WIN7     0x0601 // Windows 7
#define _WIN32_WINNT_WIN8     0x0602 // Windows 8
#define _WIN32_WINNT_WINBLUE  0x0603 // Windows 8.1
#define _WIN32_WINNT_WIN10    0x0A00 // Windows 10
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#elif defined(_POSIX_SOURCE) || defined(_XOPEN_SOURCE)

#if __has_include(<features.h>)
#include <features.h>
#endif
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <dlfcn.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define _POSIX_1990 1       // IEEE Standard 1003.1-1990
#define _POSIX_1992 2       // IEEE Standard 1003.2-1992
#define _POSIX_1993 199309L // IEEE Standard 1003.1b-1993
#define _POSIX_1995 199506L // IEEE Standard 1003.1c-1995
#define _POSIX_2001 200112L // IEEE Standard 1003.1-2002
#define _POSIX_2008 200809L // IEEE Standard 1003.1-2008

#endif

