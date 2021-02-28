#ifndef api_hpp
#define api_hpp "Application Programming Interface"

// https://sourceforge.net/p/predef/wiki/Standards/

#ifdef _WIN32

#	if __has_include("targetver.h")
#		include "targetver.h"
#	endif

#	if defined(_WIN32_WINNT) && !defined(WINVER)
#		define WINVER _WIN32_WINNT
#	elif !defined(_WIN32_WINNT) && defined(WINVER)
#		define _WIN32_WINNT WINVER
#	endif

#	if __has_include(<SDKDDKVer.h>)
#		include <SDKDDKVer.h>
#	else
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

#	if _WIN32_WINNT >= _WIN32_WINNT_NT4
#		define _NT4
#	endif
#	if _WIN32_WINNT >= _WIN32_WINNT_WIN2K
#		define _WIN2K
#	endif
#	if _WIN32_WINNT >= _WIN32_WINNT_WINXP
#		define _WINXP
#	endif
#	if _WIN32_WINNT >= _WIN32_WINNT_WS03
#		define _WS03
#	endif
#	if _WIN32_WINNT >= _WIN32_WINNT_VISTA
#		define _VISTA
#	endif
#	if _WIN32_WINNT >= _WIN32_WINNT_WIN7
#		define _WIN7
#	endif
#	if _WIN32_WINNT >= _WIN32_WINNT_WIN8
#		define _WIN8
#	endif
#	if _WIN32_WINNT >= _WIN32_WINNT_WIN81
#		define _WIN81
#	endif
#	if _WIN32_WINNT >= _WIN32_WINNT_WIN10
#		define _WIN10
#	endif

#elif defined(unix) || defined(__unix__) || defined(__unix) || __has_include(<unistd.h>)

#	if __has_include(<features.h>)
#		include <features.h>
#	else
#		include <unistd.h>
#	endif

#	define _UNISTD

#	ifdef _XOPEN_VERSION
#		define _XPG
#	endif
#	if _XOPEN_VERSION >= 2 
#		define _XPG2 // X/Open Portability Guide 2 (1985)
#	endif
#	if _XOPEN_VERSION >= 3 
#		define _XPG3 // X/Open Portability Guide 3 (1989)
#	endif
#	if _XOPEN_VERSION >= 4 
#		define _XPG4 // X/Open Portability Guide 4 (1992)
#	endif
#	ifdef _XOPEN_UNIX
#		define _SUS // X/Open Single UNIX Specification (UNIX95)
#	endif
#	if _XOPEN_VERSION >= 500 
#		define _SUS2 // X/Open Single UNIX Specification, Version 2 (UNIX98)
#	endif
#	if _XOPEN_VERSION >= 600 
#		define _SUS3 // Open Group Single UNIX Specification, Version 3 (UNIX03)
#	endif
#	if _XOPEN_VERSION >= 700 
#		define _SUS4 // Open Group Single UNIX Specification, Version 4
#	endif

#	ifdef _POSIX_VERSION
#		define _POSIX
#	endif
#	if _POSIX_C_VERSION
#		define _POSIX2
#	endif
#	if _POSIX_VERSION >= 198808L 
#		define _POSIX_1988 // IEEE 1003.1-1988
#	endif
#	if _POSIX_VERSION >= 199009L 
#		define _POSIX_1990 // ISO/IEC 9945-1:1990
#	endif
#	if _POSIX2_C_VERSION >= 199209L 
#		define _POSIX_1992 // ISO/IEC 9945-2:1993
#	endif
#	if _POSIX_VERSION >= 199309L 
#		define _POSIX_1993 // IEEE 1003.1b-1993
#	endif
#	if _POSIX_VERSION >= 199506L 
#		define _POSIX_1996 // IEEE 1003.1-1996
#	endif
#	if _POSIX_VERSION >= 200112L 
#		define _POSIX_2001 // IEEE 1003.1-2001
#	endif
#	if _POSIX_VERSION >= 200809L
#		define _POSIX_2008 // IEEE 1003.1-2008
#	endif

#else
#error "System target missing"
#endif
#endif
