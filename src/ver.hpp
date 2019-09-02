
// https://sourceforge.net/p/predef/wiki/Standards/

#ifdef _WIN32
#	if __has_include("targetver.h")
#		include "targetver.h"
#	endif

#	if defined(_WIN32_WINNT) && !defined(WINVER)
#		define WINVER _WIN32_WINNT
#	elif defined(WINVER)
#		define _WIN32_WINNT WINVER
#	endif

#	if __has_include(<SDKDDKVer.h>)
#		include <SDKDDKVer.h>
	#else
#		define _WIN32_WINNT_NT4      0x0400 // Windows NT 4.0
#		define _WIN32_WINNT_WIN2K    0x0500 // Windows 2000
#		define _WIN32_WINNT_WINXP    0x0501 // Windows XP
#		define _WIN32_WINNT_WS03     0x0502 // Windows Server 2003
#		define _WIN32_WINNT_VISTA    0x0600 // Windows Vista
#		define _WIN32_WINNT_WIN7     0x0601 // Windows 7
#		define _WIN32_WINNT_WIN8     0x0602 // Windows 8
#		define _WIN32_WINNT_WINBLUE  0x0603 // Windows 8.1
#		define _WIN32_WINNT_WIN10    0x0A00 // Windows 10
#	endif

#	define _NT4   _WIN32_WINNT_NT4     <= _WIN32_WINNT
#	define _WIN2K _WIN32_WINNT_WIN2K   <= _WIN32_WINNT
#	define _WINXP _WIN32_WINNT_WINXP   <= _WIN32_WINNT
#	define _WIN03 _WIN32_WINNT_WS03    <= _WIN32_WINNT
#	define _VISTA _WIN32_WINNT_VISTA   <= _WIN32_WINNT
#	define _WIN7  _WIN32_WINNT_WIN7    <= _WIN32_WINNT
#	define _WIN8  _WIN32_WINNT_WIN8    <= _WIN32_WINNT
#	define _WIN81 _WIN32_WINNT_WINBLUE <= _WIN32_WINNT
#	define _WIN10 _WIN32_WINNT_WIN10   <= _WIN32_WINNT

#elif defined(unix) || defined(__unix__) || defined(__unix) || __has_include(<unistd.h>)
#	if __has_include(<features.h>)
#		include <features.h>
#	else
#		include <unistd.h>
#	endif

#	define _UNISTD
#	define _POSIX      1       <= _POSIX_VERSION
#	define _POSIX2     1       <= _POSIX_C_VERSION
#	define _XOPEN      1       <= _XOPEN_VERSION
#	define _XPG2       2       <= _XOPEN_VERSION    // X/Open Portability Guide 2 (1985)
#	define _XPG3       3       <= _XOPEN_VERSION    // X/Open Portability Guide 3 (1989)
#	define _XPG4       4       <= _XOPEN_VERSION    // X/Open Portability Guide 4 (1992)
#	define _SUS2       500     <= _XOPEN_VERSION    // X/Open Single UNIX Specification, Version 2 (UNIX98)
#	define _SUS3       600     <= _XOPEN_VERSION    // Open Group Single UNIX Specification, Version 3 (UNIX03)
#	define _SUS4       700     <= _XOPEN_VERSION    // Open Group Single UNIX Specification, Version 4
#	define _POSIX_1988 198808L <= _POSIX_VERSION    // IEEE 1003.1-1988
#	define _POSIX_1990 199009L <= _POSIX_VERSION    // ISO/IEC 9945-1:1990
#	define _POSIX_1992 199209L <= _POSIX2_C_VERSION // ISO/IEC 9945-2:1993
#	define _POSIX_1993 199309L <= _POSIX_VERSION    // IEEE 1003.1b-1993
#	define _POSIX_1996 199506L <= _POSIX_VERSION    // IEEE 1003.1-1996
#	define _POSIX_2001 200112L <= _POSIX_VERSION    // IEEE 1003.1-2001
#	define _POSIX_2008 200809L <= _POSIX_VERSION    // IEEE 1003.1-2008

#else
#error "System target missing"
#endif
