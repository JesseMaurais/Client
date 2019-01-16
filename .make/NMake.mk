
# Guess the environment

!ifdef SHELL
# Probably POSIX
include .make/SH.mk
!elseifdef COMSPEC
# Probably WIN32
include .make/CMD.mk
!else
# Bad way
!error Cannot determine the operating environment 
!endif

# Guess the compiler

!if "$(CXX)" != "$(CXX:clang=)"
include .make/Clang.mk
!elseif "$(CXX)" != "$(CXX:g++=)"
!message $(BUGMSG)
include .make/GCC.mk
!elseif "$(CXX)" != "$(CXX:cl=)"
include .make/CL.mk
!else
!message Cannot determine your compiler
!endif

# Custom configurations

CFLAGS = $(CFLAGS) $(DEF) $(FLAGS) $(WARN)

!ifdef CA
CFLAGS = $(CFLAGS) $(ANALYZE)
!endif

!ifdef STD
CFLAGS = $(CFLAGS) $(USESTD)$(STD)
!endif

!ifndef NDEBUG
CFLAGS = $(CFLAGS) $(DEBUG)
!endif

