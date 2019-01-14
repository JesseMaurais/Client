
# Guess the environment

ifdef COMSPEC
# Probably WIN32
include .make/CMD.mk
else
ifdef SHELL
# Probably POSIX
include .make/SH.mk
else
# Bad way
$(error Cannot determine the operating environment)
endif # SHELL
endif # COMSPEC

# Guess the compiler used

ifneq ($(findstring clang, $(CXX)),)
include .make/Clang.mk
else
ifneq ($(findstring g++, $(CXX)),)
include .make/GCC.mk
else
ifneq ($(findstring cl, $(CXX)),)
include .make/CL.mk
else
$(warning Cannot determine your compiler flags)
endif # CL
endif # GCC
endif # Clang

# Compiler commands

CFLAGS += $(FLAGS)

ifdef STD
CFLAGS += $(STDFLAG)$(STD)
endif 

ifndef NDEBUG
CFLAGS += $(DEBUG)
endif


