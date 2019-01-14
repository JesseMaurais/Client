
# Assume the environment

include .make/CMD.mk

# Assume the compiler

include .make/CL.mk

# Custom configurations

CFLAGS = $(FLAGS)

!ifndef NDEBUG
CFLAGS = $(CFLAGS) $(DEBUG)
!endif

!ifdef STD
CFLAGS = $(CFLAGS) $(USESTD)$(STD)
!endif

