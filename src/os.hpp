#ifndef os_hpp
#define os_hpp

// https://sourceforge.net/p/predef/wiki/OperatingSystems

// Microsoft Windows
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
# undef _WIN32
# define _WIN32
#endif

// Microsoft DOS
#if defined(MSDOS) || defined(_MSDOS) || defined(__MSDOS__) || defined(__DOS__)
# undef _MSDOS
# define _MSDOS
#endif

// GNU
#if defined(__GNU__) || defined(__gnu_hurd__) || defined(__gnu_linux__)
# undef _GNU
# define _GNU
#endif

// Linux
#if defined(linux) || defined(__linux) || defined(__linux__)
# undef _LINUX
# define _LINUX
#endif

// Google Android
#if defined(__ANDROID__)
# undef _ANDROID
# define _ANDROID
#endif

// Apple Mac OS
#if defined(__APPLE__) && defined(__MACH__)
# undef _MACOS
# define _MACOS
#endif

// FreeBSD
#if defined(__FreeBSD__)
# undef _FREEBSD
# define _FREEBSD
#endif

// NetBSD
#if defined(__NetBSD__)
# undef _NETBSD
# define _NETBSD
#endif

// OpenBSD
#if defined(__OpenBSD__)
# undef _OPENBSD
# define _OPENBSD
#endif

// DragonFly
#if defined(__DragonFly__)
# undef _DRAGONFLY
# define _DRAGONFLY
#endif

// OpenVMS
#if defined(VMS) || defined(__VMS)
# undef _VMS
# define _VMS
#endif

// SCO OpenServer
#if defined(M_I386) || defined(M_XENIX) || defined(_SCO_DS)
# undef _OPENSERVER
# define _OPENSERVER
#endif

// UnixWare
#if defined(sco) || defined(_UNIXWARE7)
# undef _UNIXWARE
# define _UNIXWARE
#endif

// System V
#if defined(__sysv__) || defined(__SVR4) || defined(__svr4__)
# undef _SYSV
# define _SYSV
#endif

// Oracle Solaris
#if defined(sun) || defined(__sun)
# if defined(__SYSV__)
#  undef _SOLARIS
#  define _SOLARIS
# else
#  undef _SUNOS
#  define _SUNOS
# endif
#endif

// Hewlett Packard Unix
#if defined(hpux) || defined(_hpux) || defined(__hpux)
# undef _HPUX
# define _HPUX
#endif

// SGI IRIX
#if defined(sgi) || defined(__sgi)
# undef _IRIX
# define _IRIX
#endif

// IBM AIX
#if defined(_AIX)
# undef _AIX
# define _AIX
#endif

// Berkeley Software Distribution
#if defined(__bsdi__)
# undef _BSD
# define _BSD
#endif

// Legacy UNIX
#if defined(unix) || defined(__unix) || defined(__unix__)
# undef _UNIX
# define _UNIX
#endif

#endif // file
