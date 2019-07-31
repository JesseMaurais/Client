
# Guess the operating system

!if "$(OS)" == "Windows_NT"
DEF=$(DEF) -D_WIN32
EXE=.exe
!else
DEF=$(DEF) -D_POSIX_C_SOURCE
LNK=$(LNK) -ldl -lrt
!endif

# Guess the environment

!ifdef SHELL
include .make/SH.mk
!elseifdef COMSPEC
include .make/CMD.mk
!else
!error Cannot determine the operating environment 
!endif

# Guess the compiler

!if "$(CXX)" != "$(CXX:clang=)"
include .make/LLVM.mk
!elseif "$(CXX)" != "$(CXX:g++=)"
include .make/GCC.mk
!elseif "$(CXX)" != "$(CXX:cl=)"
include .make/CL.mk
!else
!message Cannot determine your compiler
!endif

# Custom configurations

CFLAGS=$(CFLAGS) $(DEF) $(FLAGS) $(WARN)

!ifdef CA
CFLAGS=$(CFLAGS) $(ANAL)
!endif

!ifdef STD
CFLAGS=$(CFLAGS) $(USESTD)$(STD)
!endif

!ifndef NDEBUG
CFLAGS=$(CFLAGS) $(DEBUG)
!endif

