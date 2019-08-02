
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
# define cat(x, y) x=$(x) y
#else // GNU Make
# define error(x) $(error x)
# define print(x) $(info x)
# define cat(x, y) x += y
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

