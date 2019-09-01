
// https://sourceforge.net/p/predef/wiki/Standards/

#ifdef _WIN32
#	if defined(__has_include) && __has_include("targetver.h")
#	include "targetver.h"
#endif

#if defined(_WIN32_WINNT) && !defined(WINVER)
#	define WINVER _WIN32_WINNT
#	elif defined(WINVER)
#	define _WIN32_WINNT WINVER
#else
#	error "Win32 target missing"
#endif

#if defined(_has_include) && __has_include(<SDKDDKVer.h>)
#	include <SDKDDKVer.h>
#else
#	define _WIN32_WINNT_NT4      0x0400 // Windows NT 4.0
#	define _WIN32_WINNT_WIN2K    0x0500 // Windows 2000
#	define _WIN32_WINNT_WINXP    0x0501 // Windows XP
#	define _WIN32_WINNT_WS03     0x0502 // Windows Server 2003
#	define _WIN32_WINNT_VISTA    0x0600 // Windows Vista
#	define _WIN32_WINNT_WIN7     0x0601 // Windows 7
#	define _WIN32_WINNT_WIN8     0x0602 // Windows 8
#	define _WIN32_WINNT_WINBLUE  0x0603 // Windows 8.1
#	define _WIN32_WINNT_WIN10    0x0A00 // Windows 10
#endif

#define _WIN98 !(_NT4)
#define _NT4   _WIN32_WINNT_NT4     <= _WIN32_WINNT
#define _WIN2k _WIN32_WINNT_WIN2K   <= _WIN32_WINNT
#define _WINXP _WIN32_WINNT_WINXP   <= _WIN32_WINNT
#define _WIN03 _WIN32_WINNT_WS03    <= _WIN32_WINNT
#define _VISTA _WIN32_WINNT_VISTA   <= _WIN32_WINNT
#define _WIN7  _WIN32_WINNT_WIN7    <= _WIN32_WINNT
#define _WIN8  _WIN32_WINNT_WIN8    <= _WIN32_WINNT
#define _WIN81 _WIN32_WINNT_WINBLUE <= _WIN32_WINNT
#define _WIN10 _WIN32_WINNT_WIN10   <= _WIN32_WINNT

#elif defined(unix) || defined(__unix__) || defined(__unix) || (defined(__has_include)&&__has_include(<unistd.h>))

#define _UNISTD
#define _XPG3 3
#define _XPG4 4
#define _SUS2 500
#define _SUS3 600
#define _SUS4 700

#if __has_include(<features.h>)
#include <features.h>
#endif

// Version
#define _POSIX_1988 198808L         <= _POSIX_VERSION
#define _XOPEN_1989 _XPG3           <= _XOPEN_VERSION
#define _POSIX_1990 199009L         <= _POSIX_VERSION
#define _XOPEN_1992 _XPG4           <= _XOPEN_VERSION
#define _POSIX_1992 199209L         <= _POSIX2_C_VERSION
#if (_XOPEN_1992) && defined(_XOPEN_UNIX)
#	define _XOPEN_1995
#endif
#define _POSIX_1993 199309L         <= _POSIX_VERSION
#define _POSIX_1996 199506L         <= _POSIX_VERSION
#define _XOPEN_1998 _SUS2           <= _XOPEN_VERSION
#define _POSIX_2001 200112L         <= _POSIX_VERSION
#define _XOPEN_2003 _SUS3           <= _POSIX_VERSION
#define _POSIX_2008 200809L         <= _POSIX_VERSION
#define _XOPEN_2008 _SUS4           <= _XOPEN_VERSION

// Legacy
#define _UNISTD
#define _POSIX      _POSIX_SOURCE   <= _POSIX_VERSION
#define _POSIX2     _POSIX_C_SOURCE <= _POSIX_C_SOURCE
#define _XOPEN      _XOPEN_SOURCE   <= _XOPEN_VERSION
#define _LINUX      _LINUX_BASE     <= __LSB_VERSION__
#if (_POSIX) || (_XOPEN) || (_LINUX)
#define _UNI86
#endif

#else
#error "System target missing"
#endif
