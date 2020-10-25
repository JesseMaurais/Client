/*
	This C file is a Makefile template that will generate a valid Makefile
	tailored for your compiler and operating system by running it through
	the preprocessor of your compiler. By default the generated syntax
	will be compatible with GNU Make. In order to make a Makefile that is
	compatible with Microsoft NMake define the -D_NMAKE macro in CFLAGS
	or CL or else write a Tools.ini file in the directory with

		[NMAKE]
		MAKECONFIG=-D_NMAKE

	and a bootstrapping Makefile written in the portable Makefile syntax

		MAKEFILE=.make
		TEMPLATE=.make.cc
		all: $(MAKEFILE)
			$(MAKE) -f $(MAKEFILE)
		clean: $(MAKEFILE)
			$(MAKE) -f $(MAKEFILE) clean
		$(MAKEFILE): $(TEMPLATE)
			$(CXX) $(MAKECONFIG) -E $(TEMPLATE) > $(MAKEFILE)

	where MAKEFILE can be named anything you want but here is a hidden file
	and TEMPLATE is this file. In this case type `nmake` or `make` so that
	your choice of make program generates its own Makefile without any
	additional arguments. It will then build your targets. Both versions
	of the make program have the same supported feature set in this file.
		- pre compiled headers
		- locating of source code
		- source dependency generation
		- platform detection
		- auto name target program
		- uses CXX for the C++ compiler
		- uses STD for language standard
		- uses LIB for link libraries
		- uses INCLUDE for finding headers
		- uses WINVER for supported WIN32 platform
		- uses UNIVER for supported POSIX platform
*/

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
add(CFLAGS, -D_WIN32)
ifdef WINVER
add(CFLAGS, "-D_WIN32_WINNT=$(WINVER)")
endif
OUTEXT=exe
RM=del /f
MKD=md
DIR=\ //
ENT=;
else // POSIX
add(CFLAGS, -D_POSIX_SOURCE)
ifdef UNIVER
add(CFLAGS, "-D_XOPEN_SOURCE=$(UNIVER)")
endif
add(LDFLAGS, -lm -ldl -lrt -lpthread)
OUTEXT=out
RM=rm -f
MKD=mkdir -p
DIR=/
ENT=:
endif

// Project
ifndef STD
#ifdef _MSC_VER
STD=c++latest
#else
STD=c++20
#endif
endif
MAKDIR=obj$(DIR)
OBJDIR=obj$(DIR)
SRCDIR=src$(DIR)
HDRDIR=$(SRCDIR)
DEPDIR=$(OBJDIR)
PCH=pre
SRCEXT=cpp
HDREXT=hpp
MAKEXT=mak
ALLSRC=$(SRCDIR)*.$(SRCEXT)
ALLHDR=$(SRCDIR)*.$(HDREXT)

.SUFFIXES: .$(SRCEXT) .$(HDREXT) .$(MAKEXT)

// Source
#ifdef _NMAKE
ifdef COMSPEC
MAKHDR=$(MAKDIR)header.$(MAKEXT)
!if ![(echo HDR=\>$(MAKHDR)) && for %i in ($(ALLHDR)) do @echo %i\>>$(MAKHDR)]
!include $(MAKHDR)
endif // HDR
MAKSRC=$(MAKDIR)source.$(MAKEXT)
!if ![(echo SRC=\>$(MAKSRC)) && for %i in ($(ALLSRC)) do @echo %i\>>$(MAKSRC)]
!include $(MAKSRC)
endif // SRC
MAKEXE=$(MAKDIR)target.$(MAKEXT)
!if ![(echo EXE=\>$(MAKEXE)) && for /f %i in ('findstr /s /m "\<main\>" *.$(SRCEXT)') do @echo %~ni.$(OUTEXT)\>>$(MAKEXE)]
!include $(MAKEXE)
endif // EXE
MAKINC=$(MAKDIR)include.$(MAKEXT)
!if ![(echo INC=\>$(MAKINC)) && for %i in ("%INCLUDE:$(ENT)=" "%") do @echo , -I"%~i"\>>$(MAKINC)]
!include $(MAKINC)
endif // INC
#ifndef _MSC_VER
MAKINC=$(MAKDIR)include.$(MAKEXT)
!if ![(echo INC=\>$(MAKINC)) && for %i in ("%INCLUDE:$(ENT)=" "%") do @echo , -I"%~i"\>>$(MAKINC)]
!include $(MAKINC)
endif // INC
MAKLIB=$(MAKDIR)library.$(MAKEXT)
!if ![(echo LIBRARY=\>$(MAKLIB)) && for %i in ("%LIB:$(ENT)=" "%") do @echo , -L"%~i"\>>$(MAKLIB)]
!include $(MAKLIB)
#endif
endif // INC
endif // COMSPEC
#else // GNU
HDR=$(wildcard $(ALLHDR))
SRC=$(wildcard $(ALLSRC))
EXE=$(addsuffix .$(OUTEXT), $(basename $(notdir $(shell grep -l --color=never "\bmain\b" $(SRC)))))
#ifndef _MSC_VER
INC=$(addprefix -I, "$(INCLUDE:$(ENT)=" ")")
LIBRARY=$(addprefix -L, "$(LIB:$(ENT)=" ")")
#endif // _MSC_VER
#endif // _NMAKE

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
{$(SRCDIR)}.$(SRCEXT){$(DEPDIR)}.$(DEPEXT):
	@echo $< : \> $@
	@set CMD=$(CXX) $(CFLAGS) -showIncludes -Zs -c $<
	@for /F "tokens=1,2,3,*" %%A in ('%CMD%') do @if not "%%D"=="" @echo "%%D" \>> $@
endif
{$(SRCDIR)}.$(SRCEXT){$(OBJDIR)}.$(OBJEXT):: ; $(CXXCMD)
#else // GNU
$(OBJDIR)%.$(OBJEXT): $(SRCDIR)%.$(SRCEXT); $(CXXCMD)
#endif

#elif defined(__GNUC__) || defined(__llvm__) || defined(__clang__)

OBJEXT=o
DEPEXT=d
INLEXT=i
LIBEXT=a
PCHEXT=gch

ifdef INC
add(CFLAGS, $(INC))
endif
ifdef LIBRARY
add(LDFLAGS, $(LIBRARY))
endif

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

.SUFFIXES: .$(OUTEXT) .$(OBJEXT) .$(DEPEXT) .$(LIBEXT) .$(INLEXT) .$(PCHEXT)

// Outputs
#ifdef _NMAKE
ifdef COMSPEC
MAKDEP=$(MAKDIR)depend.$(MAKEXT)
!if ![(echo DEP=\>$(MAKDEP)) && for %I in ($(ALLSRC)) do @echo $(OBJDIR)%~nI.$(DEPEXT)\>>$(MAKDEP)]
!include $(MAKDEP)
endif // DEP
MAKOBJ=$(MAKDIR)object.$(MAKEXT)
!if ![(echo OBJ=\>$(MAKOBJ)) && for %I in ($(ALLSRC)) do @echo $(OBJDIR)%~nI.$(OBJEXT)\>>$(MAKOBJ)]
!include $(MAKOBJ)
endif // OBJ
endif // COMSPEC
#else // GNU
DEP=$(patsubst $(SRCDIR)%.$(SRCEXT), $(OBJDIR)%.$(DEPEXT), $(SRC))
OBJ=$(patsubst $(SRCDIR)%.$(SRCEXT), $(OBJDIR)%.$(OBJEXT), $(SRC))
#endif

// Rules
all: $(EXE)
$(EXE): $(LNKDEP); $(LNKCMD)
$(OBJDIR): ; $(MKD) $(OBJDIR)
clean: ; $(RM) $(EXE) $(OBJ) $(PCHOBJ) $(DEP) 

// Depend
#ifdef _NMAKE
ifdef COMSPEC
MAKALLDEP=$(MAKDIR)all.$(MAKEXT)
!if ![(for %i in ($(DEPDIR)*.$(DEPEXT)) do @echo !include %i) > $(MAKALLDEP)]
!include $(MAKALLDEP)
endif
endif
#else // GNU
-include $(DEP)
#endif

