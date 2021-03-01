/*
	This C file is a Makefile template that will generate a valid Makefile
	tailored for your compiler and operating system by running it through
	the preprocessor of your compiler. By default the generated syntax
	will be compatible with GNU Make. In order to create a Makefile that is
	compatible with Microsoft NMake define the -D_NMAKE macro in the command
	line or CFLAGS/CL or else write a Tools.ini file in the directory with

		[NMAKE]
		MAKECONFIG=-D_NMAKE

	and a bootstrapping Makefile written in the portable Makefile syntax

		MAKEFILE=.make
		TEMPLATE=.make.cpp
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
		- uses LIBPATH for link libraries
		- uses INCLUDE for finding headers
		- uses WINVER for supported WIN32 platform
		- uses UNIVER for supported POSIX platform
*/

#ifdef _NMAKE
# define ifdef !ifdef
# define ifndef !ifndef
# define else !else
# define endif !endif
# define include !include
# define if(x) !if ![x]
# define ifeq(x,y) !if #x==#y
# define ifneq(x,y) !if #x!=#y
# define error(x) !error x
# define message(x) !message x
# define add(x, y) x=$(x) y
#else // GNU
# define ifeq(x,y) ifeq (x,y)
# define ifneq(x,y) ifneq (x,y)
# define error(x) $(error x)
# define message(x) $(message x)
# define add(x, y) x+=y
#endif

//
// Project
//

ifndef STD
#ifdef _MSC_VER
STD=c++latest
#else
STD=c++20
#endif
endif
ifndef OBJDIR
OBJDIR=obj$(DIR)
endif
ifndef SRCDIR
SRCDIR=src$(DIR)
endif
ifndef HDRDIR
HDRDIR=$(SRCDIR)
endif
ifndef MAKDIR
MAKDIR=$(OBJDIR)
endif
ifndef DEPDIR
DEPDIR=$(MAKDIR)
endif
ifndef MAKEXT
MAKEXT=mak
endif
ifndef SRCEXT
SRCEXT=cpp
endif
ifndef HDREXT
HDREXT=hpp
endif
ifndef PCH
PCH=std
endif

.SUFFIXES: .$(SRCEXT) .$(HDREXT) .$(MAKEXT)

//
// Operating System
//

ifeq($(OS), Windows_NT)
add(CFLAGS, -D_WIN32)
ifdef WINVER
add(CFLAGS, "-D_WIN32_WINNT=$(WINVER)")
endif
OUTEXT=exe
CAT=type
RM=del /f
MKD=md
LIST=dir /b
DIR=\ //
ENT=;
else // POSIX
add(CFLAGS, -D_POSIX_SOURCE)
ifdef UNIVER
add(CFLAGS, "-D_XOPEN_SOURCE=$(UNIVER)")
endif
#ifdef __GNUC__
add(CFLAGS, "-DPOSIXLY_CORRECT")
#endif
OUTEXT=out
CAT=cat
RM=rm -f
MKD=mkdir -p
LIST=ls
DIR=/
ENT=:
endif

//
// Sources
//

#ifdef _NMAKE
MAKHDR=$(MAKDIR)header.$(MAKEXT)
if((echo HDR=\>$(MAKHDR)) && for %i in ($(HDRDIR)*.$(HDREXT)) do @echo %i\>>$(MAKHDR))
include $(MAKHDR)
endif
#else // GNU
HDR=$(wildcard $(HDRDIR)*.$(HDREXT))
#endif

#ifdef _NMAKE
MAKSRC=$(MAKDIR)source.$(MAKEXT)
if((echo SRC=\>$(MAKSRC)) && for %i in ($(SRCDIR)*.$(SRCEXT)) do @echo %i\>>$(MAKSRC))
include $(MAKSRC)
endif
#else // GNU
SRC=$(wildcard $(SRCDIR)*.$(SRCEXT))
#endif

#ifndef _MSC_VER
#ifdef _NMAKE
MAKINC=$(MAKDIR)include.$(MAKEXT)
if((echo INC=\>$(MAKINC)) && for "delims=$(ENT)" %i in (%INCLUDE%) do @echo , -I"%~i"\>>$(MAKINC))
include $(MAKINC)
endif
#else // GNU
INC=$(addprefix -I, "$(INCLUDE:$(ENT)=" ")")
#endif

#ifdef _NMAKE
MAKLIB=$(MAKDIR)library.$(MAKEXT)
if((echo LIB=\>$(MAKLIB)) && for "delims=$(ENT)" %i in (%LIBPATH%) do @echo , -L"%~i"\>>$(MAKLIB))
include $(MAKLIB)
endif
#else // GNU
LIB=$(addprefix -L, "$(LIBPATH:$(ENT)=" ")")
#endif
#endif // _MSC_VER

//
// Compiler
//

#ifdef _MSC_VER 

OBJEXT=obj
DEPEXT=dep
INLEXT=inl
LIBEXT=lib
PCHEXT=pch

ifdef STD
add(CFLAGS, -std:$(STD))
endif

add(CFLAGS, -nologo -DNOMINMAX -EHsc -permissive-)
add(LDFLAGS, -nologo)

ifndef NDEBUG
add(WARN, -Z7 -W4 -D_CRT_SECURE_NO_WARNINGS)
add(LDFLAGS, -Z7)
endif

ifdef PCH
PCHHDR=$(SRCDIR)$(PCH).$(HDREXT)
PCHSRC=$(SRCDIR)$(PCH).$(SRCEXT)
PCHOBJ=$(OBJDIR)$(PCH).$(OBJEXT)
PCHOUT=$(OBJDIR)$(PCH).$(PCHEXT)
$(PCHOBJ): $(PCHHDR); $(CXX) $(CFLAGS) $(WARN) -Yc$(PCH).$(HDREXT) -c $(PCHSRC) -Fo$(PCHOBJ) -Fp$(PCHOUT)
add(HEADER, -Yu$(PCH).$(HDREXT) -FI$(PCH).$(HDREXT) -Fp$(PCHOUT))
add(LDFLAGS, -Yu$(PCH).$(HDREXT))
endif

CXXCMD=$(CXX) $(CFLAGS) $(WARN) $(HEADER) -c $< -Fo$(OBJDIR)
LNKCMD=$(CXX) $(LDFLAGS) $(OBJ) -Fe$@
LNKDEP=$(PCHOBJ) $(OBJ) $(DEP)

#ifdef _NMAKE
{$(SRCDIR)}.$(SRCEXT){$(DEPDIR)}.$(DEPEXT):
	@echo $< : \> $@
	@set CMD=$(CXX) $(CFLAGS) -w -showIncludes -P -Fi"NUL" -c $<
	@for /F "tokens=1,2,3,*" %%A in ('%CMD% 2^>^&1') do @if not "%%D"=="" @echo "%%D" \>> $@

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

ifdef STD
add(CFLAGS, -std=$(STD))
endif

add(CFLAGS, -MP -MMD)
add(LDFLAGS, -rdynamic)

#if defined(__llvm__) || defined(__clang__)
add(CFLAGS, -stdlib=libc++)
add(LDFLAGS, -lc++ -lc++abi)
#endif

ifndef NDEBUG
add(WARN, -Wall -Wextra -Wpedantic -g)
endif

ifdef PCHSRC
PCHHDR=$(SRCDIR)$(PCHSRC).$(HDREXT)
PCHOUT=$(OBJDIR)$(PCHSRC).$(PCHEXT)
add(HEADER, -include $(PCHHDR) -I$(OBJDIR))
$(PCHOUT): $(PCHHDR); $(CXX) $(CFLAGS) $(WARN) -c $< -o $@
endif

CXXCMD=$(CXX) $(CFLAGS) $(WARN) $(HEADER) -c $< -o $@
LNKCMD=$(CXX) $(LDFLAGS) $(OBJ) -o $@
LNKDEP=$(PCHOUT) $(OBJ)

#endif // Compilers

//
// Outputs
//

.SUFFIXES: .$(OUTEXT) .$(OBJEXT) .$(DEPEXT) .$(LIBEXT) .$(INLEXT) .$(PCHEXT)

#ifdef _NMAKE
MAKEXE=$(MAKDIR)target.$(MAKEXT)
if((echo EXE=\>$(MAKEXE)) && for /f %i in ('findstr /s /m "\<main\>" *.$(SRCEXT)') do @echo %~ni.$(OUTEXT)\>>$(MAKEXE))
include $(MAKEXE)
endif
#else // GNU
EXE=$(addsuffix .$(OUTEXT), $(basename $(notdir $(shell grep -l --color=never "\bmain\b" $(SRC)))))
#endif

#ifdef _NMAKE
MAKOBJ=$(MAKDIR)object.$(MAKEXT)
if((echo OBJ=\>$(MAKOBJ)) && for %I in ($(SRC)) do @echo $(OBJDIR)%~nI.$(OBJEXT)\>>$(MAKOBJ))
include $(MAKOBJ)
endif
#else // GNU
OBJ=$(patsubst $(SRCDIR)%.$(SRCEXT), $(OBJDIR)%.$(OBJEXT), $(SRC))
#endif

#ifdef _NMAKE
MAKDEP=$(MAKDIR)depend.$(MAKEXT)
if((echo DEP=\>$(MAKDEP)) && for %I in ($(SRC)) do @echo $(DEPDIR)%~nI.$(DEPEXT)\>>$(MAKDEP))
include $(MAKDEP)
endif
#else // GNU
DEP=$(patsubst $(SRCDIR)%.$(SRCEXT), $(OBJDIR)%.$(DEPEXT), $(SRC))
#endif

#ifdef _NMAKE
ALLDEP=$(MAKDIR)all.$(MAKEXT)
if((echo. >$(ALLDEP)) && for %I in ($(DEP)) do @if exist "%I" @echo include %I >> $(ALLDEP))
include $(ALLDEP)
endif
#else // GNU
-include $(DEP)
#endif

//
// Targets
//

all: $(EXE)
help: ; $(CAT) Readme
clean: ; $(RM) $(OBJ) $(EXE) $(DEP) $(PCHOBJ) $(PCHOUT)
cflags: ; echo $(CFLAGS) > compile_flags.txt
ctags: ; ctags $(SRC) $(HDR)

$(EXE): $(LNKDEP); $(LNKCMD)
