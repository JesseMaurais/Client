# 1 ".make.cc"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 383 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 ".make.cc" 2
# 59 ".make.cc"
ifeq ($(OS),Windows_NT)
OUTEXT=exe
RM=del /f
MKD=md
DIR=\
ENT=;
CFLAGS+=-D_WIN32
ifdef WINVER
CFLAGS+="-D_WIN32_WINNT=$(WINVER)"
endif
else
OUTEXT=out
RM=rm -f
MKD=mkdir -p
DIR=/
ENT=:
CFLAGS+=-D_POSIX_SOURCE
ifdef UNIVER
CFLAGS+="-D_XOPEN_SOURCE=$(UNIVER)"
endif
LDFLAGS+=-lm -ldl -lrt -lpthread
endif


ifndef STD



STD=c++20

endif
MAKDIR=obj$(DIR)
OBJDIR=obj$(DIR)
SRCDIR=src$(DIR)
HDRDIR=$(SRCDIR)
PCH=pre
SRCEXT=cpp
HDREXT=hpp
MAKEXT=mak
ALLSRC=$(SRCDIR)*.$(SRCEXT)
ALLHDR=$(SRCDIR)*.$(HDREXT)

.SUFFIXES: .$(SRCEXT) .$(HDREXT) .$(MAKEXT)
# 120 ".make.cc"
HDR=$(wildcard $(ALLHDR))
SRC=$(wildcard $(ALLSRC))
EXE=$(basename $(notdir $(shell grep -l --color=never "\bmain\b" $(SRC)))).$(OUTEXT)



all: $(EXE)
# 193 ".make.cc"
OBJEXT=o
DEPEXT=d
INLEXT=i
LIBEXT=a
PCHEXT=gch


CFLAGS+=-MP -MMD
LDFLAGS+=-rdynamic
CXXFLAGS+=$(CFLAGS)


ifdef STD
CFLAGS+=-std=$(STD)
endif


CFLAGS+=-stdlib=libc++
LDFLAGS+=-lc++ -lc++abi



ifndef NDEBUG
CFLAGS+=-Wall -Wextra -Wpedantic -g
endif


ifdef HDRDIR
CFLAGS+=-I$(HDRDIR)
endif


ifdef PCH
PCHHDR=$(SRCDIR)$(PCH).$(HDREXT)
PCHOBJ=$(SRCDIR)$(PCH).$(PCHEXT)
CXXFLAGS+=-include $(PCHHDR)
$(PCHOBJ): $(PCHHDR); $(CXX) $(CFLAGS) -c $< -o $@
endif


CXXCMD=$(CXX) $(CXXFLAGS) -c $< -o $@
LNKCMD=$(CXX) $(LDFLAGS) $(OBJ) -o $@
LNKDEP=$(PCHOBJ) $(OBJ)





$(OBJDIR)%.o: $(SRCDIR)%.cpp; $(CXXCMD)




.SUFFIXES: .$(OUTEXT) .$(OBJEXT) .$(DEPEXT) .$(LIBEXT) .$(INLEXT) .$(PCHEXT)
# 261 ".make.cc"
DEP=$(patsubst $(SRCDIR)%.$(SRCEXT), $(OBJDIR)%.$(DEPEXT), $(SRC))
OBJ=$(patsubst $(SRCDIR)%.$(SRCEXT), $(OBJDIR)%.$(OBJEXT), $(SRC))



clean: ; $(RM) $(EXE) $(OBJ) $(PCHOBJ) $(DEP)
$(EXE): $(LNKDEP); $(LNKCMD)
$(OBJDIR): ; $(MKD) $(OBJDIR)
# 279 ".make.cc"
-include $(DEP)
