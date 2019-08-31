#ifdef _WIN32

// https://sourceforge.net/p/predef/wiki/Standards/

#if __has_include("targetver.h")
#include "targetver.h"
#endif

#if defined(_WIN32_WINNT) && !defined(WINVER)
#define WINVER _WIN32_WINNT
#elif defined(WINVER)
#define _WIN32_WINNT WINVER
#else
#error "Target operating system not specified"
#endif

#if __has_include(<SDKDDKVer.h>)
#include <SDKDDKVer.h>
#else
#define _WIN32_WINNT_NT4      0x0400 // Windows NT 4.0
#define _WIN32_WINNT_WIN2K    0x0500 // Windows 2000
#define _WIN32_WINNT_WINXP    0x0501 // Windows XP
#define _WIN32_WINNT_WS03     0x0502 // Windows Server 2003
#define _WIN32_WINNT_VISTA    0x0600 // Windows Vista
#define _WIN32_WINNT_WIN7     0x0601 // Windows 7
#define _WIN32_WINNT_WIN8     0x0602 // Windows 8
#define _WIN32_WINNT_WINBLUE  0x0603 // Windows 8.1
#define _WIN32_WINNT_WIN10    0x0A00 // Windows 10
#endif

#elif defined(_POSIX_SOURCE) || defined(_XOPEN_SOURCE)

#if __has_include(<features.h>)
#include <features.h>
#endif

#define _POSIX_1988 198808L <= _POSIX_VERSION
#define _POSIX_1990 199009L <= _POSIX_VERSION    // ISO/IEC 9945-1:1990
#define _POSIX_1992 199209L <= _POSIX2_C_VERSION // ISO/IEC 9945-2:1993
#define _POSIX_1993 199309L <= _POSIX_VERSION    // IEEE 1003.1b-1993
#define _POSIX_1996 199506L <= _POSIX_VERSION    // IEEE 1003.1c-1995
#define _POSIX_2001 200112L <= _POSIX_VERSION    // IEEE 1003.1-2002
#define _POSIX_2008 200809L <= _POSIX_VERSION    // IEEE 1003.1-2008

#endif

#define NOT_SUPPORTED "Target system does not support required features"
