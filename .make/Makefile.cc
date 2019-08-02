
// Harmonize Syntax

#define str(x) #x

#ifdef _NMAKE
# define ifdef !ifdef
# define ifndef !ifndef
# define else !else
# define endif !endif
# define ifeq(x, y) str(x) == str(y)
# define ifneq(x, y) str(x) != str(y)
# define error(x) !error x
# define print(x) !message x
# define append(x, y) x=$(x) y
# define include_if(x)
#else // GNU Make
# define error(x) $(error x)
# define print(x) $(info x)
# define append(x, y) x += y
# define include_if(x) -include x
#endif

// Compiler

#if defined(__clang__) || defined(__llvm__)
# include "LLVM.mk"
#elif defined(__GNUC__)
# include "GCC.mk"
#elif defined(_MSC_VER)
# include "CL.mk"
#elif defined(__MINGW32__) || defined(__MINGW64__)
# error "MINGW.mk not implemented."
#elif defined(__DJGPP__) || defined(__GO32__)
# error "DJGPP.mk not implemented."
#elif defined(__DMC__)
# error "DMC.mk not implemented."
#elif defined(__INTEL_COMPILER) || defined(__ICC) || defined(__ECC) || defined(__ICL)
# error "ICC.mk not implemented."
#elif defined(__IBMC__) || defined(__IBMCPP__) || defined(__xlc__) || defined(__xlC__)
# error "IBM.mk not implemented.m"
#else
# error "Cannot determine your compiler."
#endif

// Operating System

#ifdef _WIN32
# include "WIN32.mk"
#else
# include "POSIX.mk"
#endif

// System Commands

ifdef SHELL
include .make/SH.mk
else
ifdef COMSPEC
include .make/CMD.mk
else
error("Cannot determine your system commands.")
endif // COMSPEC
endif // SHELL

// Rules

#include "../Tools.mk"

