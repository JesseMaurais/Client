#ifndef cc_hpp
#define cc_hpp

// https://sourceforge.net/p/predef/wiki/Compilers/

namespace cc 
{

// GNU Compiler Collection
constexpr bool GNU =
#if defined(__GNUC__)
	true
#else
	false
#endif
	;

// Minimalist GNU for Windows
constexpr bool MINGW =
#if defined(__MINGW32__) || defined(__MINGW64__)
# undef __MINGW__
# define __MINGW__ 1
	true
#else
	false
#endif
	;

// DJ's GNU Programming Platform
constexpr bool DJGPP =
#if defined(__DJGPP__) || defined(__GO32__)
# undef __DJGPP__
# define __DJGPP__ 1
	true
#else
	false
#endif
	;

// Clang
constexpr bool CLANG =
#if defined(__clang__)
	true
#else
	false
#endif
	;

// LLVM
constexpr bool LLVM =
#if defined(__llvm__)
	true
#else
	false
#endif
	;

// Digital Mars
constexpr bool DMC =
#if defined(__DMC__)
	true
#else
	false
#endif
	;

// Microsoft Visual C++
constexpr bool MSVC =
#if defined(_MSC_VER)
# undef __MSVC__
# define __MSVC__
	true
#else
	false
#endif
	;

// IBM
constexpr bool IBM =
#if defined(__IBMC__) || defined(__IBMCPP__) || defined(__xlc__) || defined(__xlC__)
	true
#else
	false
#endif
	;

// Intel
constexpr bool INTEL =
#if defined(__INTEL_COMPILER) || defined(__ICC) || defined(__ECC) || defined(__ICL)
	true
#else
	false
#endif
	;

}

#endif // file
