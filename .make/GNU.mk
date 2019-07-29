
# Guess the operating system

ifneq ($(findstring Windows_NT, $(OS)),)
DEF += -D_WIN32
EXE = .exe
else
DEF += -D_POSIX_C_SOURCE
LNK += -ldl -lrt -lpthread
endif # OS

# Guess the environment

ifdef SHELL
include .make/SH.mk
else
ifdef COMSPEC
include .make/CMD.mk
else
$(error Cannot determine the operating environment)
endif # SHELL
endif # COMSPEC

# Guess the compiler used

ifneq ($(findstring clang, $(CXX)),)
include .make/LLVM.mk
else
ifneq ($(findstring g++, $(CXX)),)
include .make/GCC.mk
else
ifneq ($(findstring cl, $(CXX)),)
include .make/CL.mk
else
$(warning Cannot determine your compiler)
endif # CL
endif # GCC
endif # LLVM

# Compiler commands

CFLAGS += $(DEF) $(FLAGS) $(WARN)

ifdef CA
CFLAGS += $(ANAL)
endif

ifdef STD
CFLAGS += $(USESTD)$(STD)
endif 

ifndef NDEBUG
CFLAGS += $(DEBUG)
endif

