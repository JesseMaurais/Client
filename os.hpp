#ifndef os_hpp
#define os_hpp

// https://sourceforge.net/p/predef/wiki/OperatingSystems

namespace sys
{

// Microsoft Windows
constexpr bool win32 =
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
# undef __WIN32__
# define __WIN32__ 1
	true
#else
	false
#endif
	;

// Microsoft DOS
constexpr bool msdos =
#if defined(MSDOS) || defined(_MSDOS) || defined(__MSDOS__) || defined(__DOS__)
# undef __MSDOS__
# define __MSDOS__ 1
	true
#else
	false
#endif
	;

// GNU
constexpr bool gnu =
#if defined(__GNU__) || defined(__gnu_hurd__) || defined(__gnu_linux__)
# undef __GNU__
# define __GNU_ 1
	true
#else
	false
#endif
	;

// Linux
constexpr bool linux =
#if defined(linux) || defined(__linux) || defined(__linux__)
# undef __LINUX__
# define __LINUX__ 1
	true
#else
	false
#endif
	;

// Google Android
constexpr bool android =
#if defined(__ANDROID__)
# undef __ANDROID__
# define __ANDROID__ 1
	true
#else
	false
#endif
	;

// Apple Mac OS
constexpr bool macos =
#if defined(__APPLE__) && defined(__MACH__)
# undef __MACOS__
# define __MACOS__ 1
	true
#else
	false
#endif
	;

// FreeBSD
constexpr bool freebsd =
#if defined(__FreeBSD__)
# undef __FREEBSD__
# define __FREEBSD__ 1
	true
#else
	false
#endif
	;

// NetBSD
constexpr bool netbsd =
#if defined(__NetBSD__)
# undef __NETBSD__
# define __NETBSD__ 1
	true
#else
	false
#endif
	;

// OpenBSD
constexpr bool openbsd =
#if defined(__OpenBSD__)
# undef __OPENBSD__
# define __OPENBSD__ 1
	true
#else
	false
#endif
	;

// DragonFly
constexpr bool dragonfly =
#if defined(__DragonFly__)
# undef __DRAGONFLY__
# define __DRAGONFLY__ 1
	true
#else
	false
#endif
	;

// OpenVMS
constexpr bool vms =
#if defined(VMS) || defined(__VMS)
# undef __VMS__
# define __VMS__ 1
	true
#else
	false
#endif
	;

// SCO OpenServer
constexpr bool openserver =
#if defined(M_I386) || defined(M_XENIX) || defined(_SCO_DS)
# undef __OPENSERVER__
# define __OPENSERVER__ 1
	true
#else
	false
#endif
	;

// UnixWare
constexpr bool unixware =
#if defined(sco) || defined(_UNIXWARE7)
# undef __UNIXWARE__
# define __UNIXWARE__ 1
	true
#else
	false
#endif
	;

// System V
constexpr bool sysv =
#if defined(__sysv__) || defined(__SVR4) || defined(__svr4__)
# undef __SYSV__
# define __SYSV__ 1
	true
#else
	false
#endif
	;

// Oracle Solaris
#if defined(sun) || defined(__sun)
# if defined(__SYSV__)
#  undef __SOLARIS__
#  define __SOLARIS__ 1
# else
#  undef __SUNOS__
#  define __SUNOS__ 1
# endif
constexpr bool solaris = sysv;
constexpr bool sunos = not sysv;
#else
constexpr bool solaris = false;
constexpr bool sunos = false;
#endif

// Hewlett Packard Unix
constexpr bool hpux =
#if defined(hpux) || defined(_hpux) || defined(__hpux)
# undef __HPUX__
# define __HPUX__ 1
	true
#else
	false
#endif
	;

// SGI IRIX
constexpr bool irix =
#if defined(sgi) || defined(__sgi)
# undef __IRIX__
# define __IRIX__ 1
	true
#else
	false
#endif
	;

// IBM AIX
constexpr bool aix =
#if defined(_AIX)
# undef __AIX__
# define __AIX__ 1
	true
#else
	false
#endif
	;

// Berkeley Software Distribution
constexpr bool bsd =
#if defined(__bsdi__)
# undef __BSD__
# define __BSD__ 1
	true
#else
	macos or freebsd or netbsd or openbsd or dragonfly
#endif
	;

// Legacy UNIX
constexpr bool unix =
#if defined(unix) || defined(__unix) || defined(__unix__)
# undef __UNIX__
# define __UNIX__ 1
	true
#else
	sysv or bsd or aix or hpux or irix
#endif
	;
}

#endif // file
