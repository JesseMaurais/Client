#ifndef arch_hpp
#define arch_hpp

// https://sourceforge.net/p/predef/wiki/Architectures/

#if defined(__alpha__) || defined(__alpha) || defined(_M_ALPHA)
# undef _ALPHA
# define _ALPHA
#endif

#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
# undef _AMD
# define _AMD
#endif

#if defined(__arm__) || defined(__arm) || defined(__aarch64__) || defined(_M_ARM)
# undef _ARM
# define _ARM
#endif

#if defined(i386) || defined(__i386) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__) || defined(__IA32__) || defined(_M_I86) || defined(_M_IX86) || defined(_X86_) || defined(__X86__) || defined(__THW_INTEL__) || defined(__I86__) || defined(__INTEL__) || defined(__386)
# undef _I86
# define _I86
#endif

#if defined(__ia64__) || defined(__ia64) || defined(_IA64) || defined(__IA64__) || defined(_M_IA64) || defined(__itanium__)
# undef _IA64
# define _IA64
#endif

#if defined(_I86) || defined(_IA64)
# undef _INTEL
# define _INTEL
#endif

#endif // file
