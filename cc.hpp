#ifndef cc_hpp
#define cc_hpp

// https://sourceforge.net/p/predef/wiki/Compilers/

// GNU Compiler Collection
#if defined(__GNUC__)
# undef __GCC__
# define __GCC__ 1
#endif

// Minimalist GNU for Windows
#if defined(__MINGW32__) || defined(__MINGW64__)
# undef __MINGW__
# define __MINGW__ 1
#endif

// DJ's GNU Programming Platform
#if defined(__DJGPP__) || defined(__GO32__)
# undef __DJGPP__
# define __DJGPP__ 1
#endif

// Clang
#if defined(__clang__)
# undef __CLANG__
# define __CLANG__ 1
#endif

// LLVM
#if defined(__llvm__)
# undef __LLVM__
# define __LLVM__ 1
#endif

// Digital Mars
#if defined(__DMC__)
# undef __DMC__
# define __DMC__ 1
#endif

// Microsoft Visual C++
#if defined(_MSC_VER)
# undef __MSVC__
# define __MSVC__ 1
#endif

// IBM
#if defined(__IBMC__) || defined(__IBMCPP__) || defined(__xlc__) || defined(__xlC__)
# undef __IBM__
# define __IBM__ 1
#endif

// Intel
#if defined(__INTEL_COMPILER) || defined(__ICC) || defined(__ECC) || defined(__ICL)
# undef __ICC__
# define __ICC__ 1
#endif


#endif // file
