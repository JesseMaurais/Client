
// Macros
#ifdef _NMAKE
# define ifdef !ifdef
# define ifndef !ifndef
# define ifeq(x,y) !if #x==#y
# define ifneq(x,y) !if #x!=#y
# define else !else
# define endif !endif
# define message(x) !message x
# define error(x) !error x
# define add(x, y) x=$(x) y
#else // GNU
# define ifeq(x,y) ifeq (x,y)
# define ifneq(x,y) ifneq (x,y)
# define message(x) $(message x)
# define error(x) $(error x)
# define add(x, y) x+=y
#endif

// System
ifeq($(OS),Windows_NT)
OUTEXT=exe
RM=del /f
MKD=md
DIR=\ // not continuation
ENT=;
ifndef WINVER
WINVER=0x0A00
endif
add(CFLAGS, -D_WIN32 -D_WIN32_WINNT=$(WINVER))
else // POSIX
OUTEXT=out
RM=rm -f
MKD=mkdir -p
DIR=/
ENT=:
ifndef UNIVER
UNIVER=700
endif
add(CFLAGS, -D_POSIX_SOURCE -D_XOPEN_SOURCE=$(UNIVER))
add(LDFLAGS, -lm -ldl -lrt -lpthread)
endif

// Project
.SUFFIXES: .cpp .hpp .mak .mk
STD=c++17
MAKDIR=make$(DIR)
OBJDIR=obj$(DIR)
SRCDIR=src$(DIR)
HDRDIR=$(SRCDIR)
SRCEXT=cpp
HDREXT=hpp
ALLSRC=$(SRCDIR)*.$(SRCEXT)
ALLHDR=$(SRCDIR)*.$(HDREXT)
PRE=pre.hpp

all: test.$(OUTEXT)

// Source
#ifdef _NMAKE
ifdef COMSPEC
MAKHDR=$(MAKDIR)Header.mak
!if ![echo HDR=\>$(MAKHDR) ] 
!if ![for %i in ($(ALLHDR)) do @echo %i\>>$(MAKHDR) ]
!include $(MAKHDR)
endif
endif
MAKSRC=$(MAKDIR)Source.mak
!if ![echo SRC=\>$(MAKSRC) ]
!if ![for %i in ($(ALLSRC)) do @echo %i\>>$(MAKSRC) ]
!include $(MAKSRC)
endif
endif
endif
#else // GNU
HDR=$(wildcard $(ALLHDR))
SRC=$(wildcard $(ALLSRC))
#endif

//
// Compiler
//

#ifdef _MSC_VER

OBJEXT=obj
DEPEXT=dep
INLEXT=inl
LIBEXT=lib
PCHEXT=pch

// Flags
add(CFLAGS, -nologo -DNOMINMAX -EHsc -permissive-)
add(LDFLAGS, -nologo)
add(CXXFLAGS, $(CFLAGS))

// Standard
ifdef STD
add(CFLAGS, -std:$(STD))
endif

// Header
ifdef HDRDIR
add(CFLAGS, -I$(HDRDIR))
endif

// Debug
ifndef NDEBUG
add(CFLAGS, -Z7 -W4 -D_CRT_SECURE_NO_WARNINGS)
add(LDFLAGS, -Z7)
endif

// Precompile
ifdef PRE
PCH=$(SRCDIR)$(PRE:.hpp=.pch)
$(PCH): $(SRCDIR)$(PRE); $(CXX) $(CFLAGS) -Yc$(PRE) -FI$(PRE) -Fe$(PCH) -c $(SRCDIR)$(PRE:.hpp=.cpp)
add(CCFLAGS, -Yu$(PCH) -FI$(PRE))
add(LDFLAGS, -Yu$(PCH))
endif

// Compile
CXXCMD=$(CXX) $(CXXFLAGS) -c $< -Fo$@

// Link
LNKCMD=$(CXX) $(LDFLAGS) $(OBJ) -Yu$(PCH) -Fe$@
LNKDEP=$(PCH) $(OBJ) $(DEP)

// Rules
#ifdef _NMAKE
ifdef COMSPEC
{$(SRCDIR)}.cpp{$(OBJDIR)}.dep:
	@echo $< : \> $@
	set CMD=$(CXX) $(CFLAGS) -showIncludes -Zs -c $<
	@for /F "tokens=1,2,3,*" %%A in ('%CMD%') do @if not "%%D"=="" @echo "%%D" \>> $@
endif
{$(OBJDIR)}.o{$(OBJDIR)}.cpp:; $(CXXCMD)
#else
$(OBJDIR)%.o: $(SRCDIR)%.cpp; $(CXXCMD)
#endif

#elif defined(__GNUC__) || defined(__llvm__) || defined(__clang__)

OBJEXT=o
DEPEXT=d
INLEXT=i
LIBEXT=a
PCHEXT=gch

// Flags
add(CFLAGS, -MP -MMD)
add(LDFLAGS, -rdynamic)
add(CXXFLAGS, $(CFLAGS))

// Standard
ifdef STD
add(CFLAGS, -std=$(STD))
endif

// Header
ifdef HDRDIR
add(CFLAGS, -I$(HDRDIR))
endif

// Debug
ifndef NDEBUG
add(CFLAGS, -Wall -Wextra -Wpedantic -g)
endif

// Precompile
ifdef PRE
PCH=$(SRCDIR)$(PRE).gch
$(PCH): $(SRCDIR)$(PRE); $(CXX) $(CFLAGS) -c $< -o $@
add(CXXFLAGS, -include $(PRE))
endif

// Compile
CXXCMD=$(CXX) $(CXXFLAGS) -c $< -o $@

// Link
LNKCMD=$(CXX) $(LDFLAGS) $(OBJ) -o $@
LNKDEP=$(PCH) $(OBJ)

// Rules
#ifdef _NMAKE
{$(OBJDIR)}.o{$(OBJDIR)}.cpp:; $(CXXCMD)
#else
$(OBJDIR)%.o: $(SRCDIR)%.cpp; $(CXXCMD)
#endif

#endif // Compile //

.SUFFIXES: .$(OBJEXT) .$(DEPEXT) .$(LIBEXT) .$(INLEXT) .$(PCHEXT)

// Outputs
#ifdef _NMAKE
ifdef COMSPEC
MAKDEP=$(MAKDIR)Depend.mak
!if ![echo DEP=\>$(MAKDEP) ] 
!if ![for %I in ($(ALLSRC)) do @echo $(OBJDIR)%~nI.$(DEPEXT)\>>$(MAKDEP) ]
!include $(MAKDEP)
endif
endif
MAKOBJ=$(MAKDIR)Object.mak
!if ![echo OBJ=\>$(MAKOBJ) ]
!if ![for %I in ($(ALLSRC)) do @echo $(OBJDIR)%~nI.$(OBJEXT)\>>$(MAKOBJ) ]
!include $(MAKOBJ)
endif
endif
endif
#else // GNU
DEP=$(patsubst $(SRCDIR)%.$(SRCEXT), $(OBJDIR)%.$(DEPEXT), $(SRC))
OBJ=$(patsubst $(SRCDIR)%.$(SRCEXT), $(OBJDIR)%.$(OBJEXT), $(SRC))
#endif

// Rules
test.$(OUTEXT): $(LNKDEP); $(LNKCMD)

// Clean
clean: ; $(RM) test.$(OUTEXT) $(OBJ) $(DEP) $(PCH)

// Depend
#ifdef _NMAKE
ifdef COMSPEC
MAKALLDEP=$(MAKDIR)All.mak
!if ![(for %i in ($(ALLDEP)) do @echo !include %i) > $(MAKALLDEP)]
!include $(MAKALLDEP)
endif
endif
#else // GNU
-include $(DEP)
#endif

