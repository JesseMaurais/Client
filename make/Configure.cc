
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
DIR=\ //
ENT=;
add(CFLAGS, -D_WIN32)
ifdef WINVER
add(CFLAGS, "-D_WIN32_WINNT=$(WINVER)")
endif
else // POSIX
OUTEXT=out
RM=rm -f
MKD=mkdir -p
DIR=/
ENT=:
add(CFLAGS, -D_POSIX_SOURCE)
ifdef UNIVER
add(CFLAGS, "-D_XOPEN_SOURCE=$(UNIVER)")
endif
add(LDFLAGS, -lm -ldl -lrt -lpthread)
endif

// Project
.SUFFIXES: .cpp .hpp .mak .mk
STD=c++20
MAKDIR=make$(DIR)
OBJDIR=obj$(DIR)
SRCDIR=src$(DIR)
HDRDIR=$(SRCDIR)
PCH=pre
SRCEXT=cpp
HDREXT=hpp
ALLSRC=$(SRCDIR)*.$(SRCEXT)
ALLHDR=$(SRCDIR)*.$(HDREXT)

all: test.$(OUTEXT)

// Source
#ifdef _NMAKE
ifdef COMSPEC
MAKHDR=$(MAKDIR)Header.mak
!if ![(echo HDR=\>$(MAKHDR)) && for %i in ($(ALLHDR)) do @echo %i\>>$(MAKHDR)]
!include $(MAKHDR)
endif // HDR
MAKSRC=$(MAKDIR)Source.mak
!if ![(echo SRC=\>$(MAKSRC)) && for %i in ($(ALLSRC)) do @echo %i\>>$(MAKSRC)]
!include $(MAKSRC)
endif // SRC
endif // COMSPEC
#else // GNU
HDR=$(wildcard $(ALLHDR))
SRC=$(wildcard $(ALLSRC))
#endif

//
// Compiler
//

#ifdef _MSC_VER // Microsoft Visual C++

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

// Debug
ifndef NDEBUG
add(CFLAGS, -Z7 -W4 -D_CRT_SECURE_NO_WARNINGS)
add(LDFLAGS, -Z7)
endif

// Header
ifdef HDRDIR
add(CFLAGS, -I$(HDRDIR))
endif

// Precompile
ifdef PCH
PCHHDR=$(SRCDIR)$(PCH).$(HDREXT)
PCHSRC=$(SRCDIR)$(PCH).$(SRCEXT)
PCHOBJ=$(OBJDIR)$(PCH).$(OBJEXT)
PCHOUT=$(SRCDIR)$(PCH).$(PCHEXT)
add(CFLAGS, -FI$(PCH).$(HDREXT) -Fp$(PCHOUT))
add(CXXFLAGS, -Yu$(PCH).$(HDREXT))
add(LDFLAGS, -Yu$(PCH).$(HDREXT))
$(PCHOBJ): $(PCHHDR); $(CXX) $(CFLAGS) -Yc$(PCH).$(HDREXT) -c $(PCHSRC) -Fo$(PCHOBJ)
endif

// Commands
CXXCMD=$(CXX) $(CXXFLAGS) -c $< -Fo$(OBJDIR)
LNKCMD=$(CXX) $(LDFLAGS) $(OBJ) -Fe$@
LNKDEP=$(PCHOBJ) $(OBJ) $(DEP)

// Rules
#ifdef _NMAKE
ifdef COMSPEC
{$(SRCDIR)}.$(SRCEXT){$(OBJDIR)}.$(DEPEXT):
	@echo $< : \> $@
	@set CMD=$(CXX) $(CFLAGS) -showIncludes -Zs -c $<
	@for /F "tokens=1,2,3,*" %%A in ('%CMD%') do @if not "%%D"=="" @echo "%%D" \>> $@
endif
{$(SRCDIR)}.$(SRCEXT){$(OBJDIR)}.$(OBJEXT):: ; $(CXXCMD)
#else
$(OBJDIR)%.obj: $(SRCDIR)%.cpp; $(CXXCMD)
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

#if defined(__llvm__) || defined(__clang__)
add(CFLAGS, -stdlib=libc++)
add(LDFLAGS, -lc++ -lc++abi)
#endif

// Debug
ifndef NDEBUG
add(CFLAGS, -Wall -Wextra -Wpedantic -g)
endif

// Header
ifdef HDRDIR
add(CFLAGS, -I$(HDRDIR))
endif

// Precompile
ifdef PCH
PCHHDR=$(SRCDIR)$(PCH).$(HDREXT)
PCHOBJ=$(SRCDIR)$(PCH).$(PCHEXT)
add(CXXFLAGS, -include $(PCHHDR))
$(PCHOBJ): $(PCHHDR); $(CXX) $(CFLAGS) -c $< -o $@
endif

// Commands
CXXCMD=$(CXX) $(CXXFLAGS) -c $< -o $@
LNKCMD=$(CXX) $(LDFLAGS) $(OBJ) -o $@
LNKDEP=$(PCHOBJ) $(OBJ)

// Rules
#ifdef _NMAKE
{$(SRCDIR)}.$(SRCEXT){$(OBJDIR)}.$(OBJEXT):: ; $(CXXCMD)
#else
$(OBJDIR)%.o: $(SRCDIR)%.cpp; $(CXXCMD)
#endif

#endif // Compilers

.SUFFIXES: .$(OBJEXT) .$(DEPEXT) .$(LIBEXT) .$(INLEXT) .$(PCHEXT)

// Outputs
#ifdef _NMAKE
ifdef COMSPEC
MAKDEP=$(MAKDIR)Depend.mak
!if ![(echo DEP=\>$(MAKDEP)) && for %I in ($(ALLSRC)) do @echo $(OBJDIR)%~nI.$(DEPEXT)\>>$(MAKDEP)]
!include $(MAKDEP)
endif // DEP
MAKOBJ=$(MAKDIR)Object.mak
!if ![(echo OBJ=\>$(MAKOBJ)) && for %I in ($(ALLSRC)) do @echo $(OBJDIR)%~nI.$(OBJEXT)\>>$(MAKOBJ)]
!include $(MAKOBJ)
endif // OBJ
endif // COMSPEC
#else // GNU
DEP=$(patsubst $(SRCDIR)%.$(SRCEXT), $(OBJDIR)%.$(DEPEXT), $(SRC))
OBJ=$(patsubst $(SRCDIR)%.$(SRCEXT), $(OBJDIR)%.$(OBJEXT), $(SRC))
#endif

// Rules
test.$(OUTEXT): $(LNKDEP); $(LNKCMD)

// Clean
clean: ; $(RM) test.$(OUTEXT) $(OBJ) $(PCHOBJ) $(DEP) 

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

// Analysis
include make/PVS.mk
analysis: pvs

