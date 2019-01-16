
# Guess the environment

ifdef SHELL
# Probably POSIX
include .make/SH.mk
else
ifdef COMSPEC
# Probably WIN32
include .make/CMD.mk
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
$(warning $(BUGMSG))
include .make/GCC.mk
else
ifneq ($(findstring cl, $(CXX)),)
include .make/CL.mk
else
$(warning Cannot determine your compiler)
endif # CL
endif # GCC
endif # Clang

# Compiler commands

CFLAGS += $(DEF) $(FLAGS) $(WARN)

ifdef CA
CFLAGS += $(ANALYZE)
endif

ifdef STD
CFLAGS += $(USESTD)$(STD)
endif 

ifndef NDEBUG
CFLAGS += $(DEBUG)
endif

