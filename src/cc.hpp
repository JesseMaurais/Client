#ifndef cc_hpp
#define cc_hpp

// https://sourceforge.net/p/predef/wiki/Compilers/

// GNU Compiler Collection
#if defined(__GNUC__)
# undef _GCC
# define _GCC
#endif

// Minimalist GNU for Windows
#if defined(__MINGW32__) || defined(__MINGW64__)
# undef _MINGW
# define _MINGW
#endif

// DJ's GNU Programming Platform
#if defined(__DJGPP__) || defined(__GO32__)
# undef _DJGPP
# define _DJGPP
#endif

// Clang
#if defined(__clang__)
# undef _CLANG
# define _CLANG
#endif

// LLVM
#if defined(__llvm__)
# undef _LLVM
# define _LLVM
#endif

// Digital Mars
#if defined(__DMC__)
# undef _DMC
# define _DMC
#endif

// Microsoft Visual C++
#if defined(_MSC_VER)
# undef _MSC
# define _MSC
#endif

// IBM
#if defined(__IBMC__) || defined(__IBMCPP__) || defined(__xlc__) || defined(__xlC__)
# undef _XLC
# define _XLC
#endif

// Intel
#if defined(__INTEL_COMPILER) || defined(__ICC) || defined(__ECC) || defined(__ICL)
# undef _ICC
# define _ICC
#endif


#endif // file
