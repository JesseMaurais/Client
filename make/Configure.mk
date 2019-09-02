# 1 "make/Configure.cc"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 382 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "make/Configure.cc" 2
# 22 "make/Configure.cc"
ifeq ($(OS),Windows_NT)
 OUT=.exe
 RM=del /f
 MKD=md
 DIR= ENT=;

 ifndef WINVER
  WINVER=0xA000
 endif
 CFLAGS += -D_WIN32 -D_WIN32_WINNT=$(WINVER)
else
 OUT=.out
 RM=rm -f
 MKD=mkdir -p
 DIR=/
 ENT=:
 ifndef UNIVER
  UNIVER=700
 endif
 CFLAGS += -D_POSIX_SOURCE -D_XOPEN_SOURCE=$(UNIVER)
 LDFLAGS += -lm -ldl -lrt -lpthread
endif


.SUFFIXES: .cpp .hpp .mak $(OUT)
STD=c++17
MAKDIR=make$(DIR)
OBJDIR=obj$(DIR)
SRCDIR=src$(DIR)
HDRDIR=$(SRCDIR)
ALLHDR=$(SRCDIR)*.hpp
ALLSRC=$(SRCDIR)*.cpp
PRE=$(SRCDIR)pre.hpp
BIN=$(SRCDIR)test.cpp

all: $(OBJDIR) test$(OUT)
$(OBJDIR): ; $(MKD) $(OBJDIR)
# 77 "make/Configure.cc"
HDR=$(wildcard $(ALLHDR))
SRC=$(wildcard $(ALLSRC))
# 153 "make/Configure.cc"
.SUFFIXES: .o .d .i .a .gch
CFLAGS += -MP -MMD
LDFLAGS += -rdynamic
CXXFLAGS += $(CFLAGS)


ifdef STD
 CFLAGS += -std=$(STD)
endif


ifdef HDRDIR
 CFLAGS += -I$(HDRDIR)
endif


ifndef NDEBUG
 CFLAGS += -Wall -Wextra -Wpedantic -g
endif


ifdef PRE
 PCH=$(PRE).gch
 $(PCH): $(PRE); $(CXX) $(CFLAGS) -c $< -o $@
 CXXFLAGS += -include $(PRE)
endif
# 197 "make/Configure.cc"
DEP=$(patsubst $(SRCDIR)%.cpp, $(OBJDIR)%.d, $(SRC))
OBJ=$(patsubst $(SRCDIR)%.cpp, $(OBJDIR)%.o, $(SRC))



test$(OUT): $(PCH) $(OBJ); $(CXX) $(LDFLAGS) $(OBJ) -o $@



$(OBJDIR)%.o: $(SRCDIR)%.cpp $(PCH); $(CXX) $(CXXFLAGS) -c $< -o $@





clean: ; $(RM) test$(OUT) $(OBJ) $(DEP) $(PCH)
# 223 "make/Configure.cc"
-include $(DEP)



.SUFFIXES: .mk
include $(MAKDIR)Cppcheck.mk
include $(MAKDIR)PVS.mk
