
# Guess the environment

!ifdef COMSPEC
# Probably WIN32
include .make/CMD.mk
!elseifdef SHELL
# Probably POSIX
include .make/SH.mk
!else
# Bad way
!error Cannot determine the operating environment 
!endif

# Guess the compiler

!if "$(CXX)" != "$(CXX:clang=)"
include .make/Clang.mk
!elseif "$(CXX)" != "$(CXX:g++=)"
include .make/GCC.mk
!elseif "$(CXX)" != "$(CXX:cl=)"
include .make/CL.mk
!else
!message Cannot determine your compiler
!endif

# Custom configurations

!ifndef NDEBUG
CFLAGS = $(CFLAGS) $(DEBUG)
!endif

CFLAGS = $(CFLAGS) $(FLAGS)

!ifdef STD
CFLAGS = $(CFLAGS) $(USESTD)$(STD)
!endif

