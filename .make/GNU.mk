
ifneq ($(findstring g++, $(CXX)),)
-include .make/GCC.mk
endif

ifneq ($(findstring clang++, $(CXX)),) 
-include .make/Clang.mk
endif

ifdef STD
override CFLAGS += -std=$(STD)
endif

COUT = -o 

EXEEXT =
OBJEXT = .o

DEL = rm -f
