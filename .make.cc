/*
	This C file is a Makefile template that will generate a valid Makefile
	tailored for your compiler and operating system by running it through
	the preprocessor of your compiler. By default the generated syntax
	will be compatible with GNU Make. In order to create a Makefile that is
	compatible with Microsoft NMake define the -D_NMAKE macro in the command
	line or CFLAGS/CL or else write a Tools.ini file in the directory with

		[NMAKE]
		MAKECONFIG=-D_NMAKE

	and/or a bootstrapping Makefile written in the portable Makefile syntax

		MAKEFILE=.make
		TEMPLATE=.make.cpp
		# GNU line continuation \
		MAKCONFIG=-D_NMAKE
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
		- source dependency generation
		- platform detection
		- locating of source code
		- auto name target program
		- uses CXX for the C++ compiler
		- uses STD for language standard
		- uses INCLUDE for finding headers
		- uses LIBPATH for link libraries
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
# define ifexists(x) !if exists(x)
# define error(x) !error x
# define message(x) !message x
# define add(x, y) x=$(x) y
#else // GNU
# define if(x) ifeq(0, $(shell x 1>$(NULL); echo $$?))
# define ifeq(x,y) ifeq (x,y)
# define ifneq(x,y) ifneq (x,y)
# define ifexists(x) ifeq ($(wildcard x),)
# define error(x) $(error x)
# define message(x) $(message x)
# define add(x, y) x+=y
#endif

//
// Project Layout
//

ifndef STD
#ifdef _MSC_VER
STD=c++latest
#else
STD=c++20
#endif
endif

ifndef MAKEXT
MAKEXT=.mak
endif
ifndef SRCEXT
SRCEXT=.cpp
endif
ifndef HDREXT
HDREXT=.hpp
endif

ifndef OBJDIR
OBJDIR=object$(DIR)
endif
ifndef SRCDIR
SRCDIR=source$(DIR)
endif
ifndef HDRDIR
HDRDIR=include$(DIR)
endif
ifndef MAKDIR
MAKDIR=$(OBJDIR)
endif
ifndef DEPDIR
DEPDIR=$(MAKDIR)
endif

ifndef PCH
ifexists($(SRCDIR)std$(HDREXT))
PCH=std
else
ifexists($(SRCDIR)stdafx$(HDREXT))
PCH=stdafx
endif // AFX
endif // STD
endif // PCH

ifdef INCLUDE
INCLUDE=$(INCLUDE)$(ENT)$(HDRDIR)
else
INCLUDE=$(HDRDIR)
endif

ifdef LIBPATH
LIBPATH=$(LIBPATH)$(ENT)$(OBJDIR)
else
LIBPATH=$(OBJDIR)
endif

//
// Operating System
//

ifeq($(OS), Windows_NT)
add(CFLAGS, -D_WIN32)
ifdef WINVER
add(CFLAGS, "-D_WIN32_WINNT=$(WINVER)")
endif
EXEEXT=.exe
DIR=\ //
ENT=;
else // POSIX
add(CFLAGS, -D_POSIX_SOURCE)
ifdef UNIVER
add(CFLAGS, "-D_XOPEN_SOURCE=$(UNIVER)")
endif
EXEEXT=.out
DIR=/
ENT=:
endif

//
// Command Line
//

ifdef COMSPEC
CMD=$(COMSPEC)
NULL=nul
WHERE=where
LIST=dir /b
SHOW=type
COPY=copy
MOVE=move
REMOVE=del /f
MKDIR=md
MKTEMP=mktemp
else
CMD=$(SHELL)
NULL=/dev/null
WHERE=which -a
LIST=ls
SHOW=cat
COPY=cp
MOVE=mv
REMOVE=rm -f
MKDIR=mkdir -p
MKTEMP=mktemp
endif

//
// Input Sources
//

#ifdef _NMAKE
MAKHDR=$(MAKDIR)header$(MAKEXT)
if((echo HDR=\>$(MAKHDR)) && for %i in ($(HDRDIR)*$(HDREXT)) do @echo %i\>>$(MAKHDR))
include $(MAKHDR)
else
error(Cannot locate header files)
endif
#else // GNU
HDR=$(wildcard $(HDRDIR)*$(HDREXT))
#endif

#ifdef _NMAKE
MAKSRC=$(MAKDIR)source$(MAKEXT)
if((echo SRC=\>$(MAKSRC)) && for %i in ($(SRCDIR)*$(SRCEXT))) do @echo %i\>>$(MAKSRC))
include $(MAKSRC)
else
error(Cannot locate source files)
endif
#else // GNU
SRC=$(wildcard $(SRCDIR)*$(SRCEXT))
#endif

#ifndef _MSC_VER

#ifdef _NMAKE
MAKINC=$(MAKDIR)includes$(MAKEXT)
if((echo INC=\>$(MAKINC)) && for %i in ("%INCLUDE:;=";"%") do @echo -I"%~i"\>>$(MAKINC))
include $(MAKINC)
else
error(Cannot parse include directories)
endif
#else // GNU
INC=$(addprefix -I, "$(INCLUDE:$(ENT)=" ")")
#endif
add(HEAD, $(INC))

#ifdef _NMAKE
MAKLIB=$(MAKDIR)library$(MAKEXT)
if((echo LIB=\>$(MAKLIB)) && for %i in ("%LIBPATH:;=";"%") do @echo -L"%~i"\>>$(MAKLIB))
include $(MAKLIB)
else
error(Cannot parse library directories)
endif
#else // GNU
LIB=$(addprefix -L, "$(LIBPATH:$(ENT)=" ")")
#endif
add(LINK, $(LIB))

#endif//_MSC_VER

//
// Output Objects
//

#ifdef _NMAKE
MAKEXE=$(MAKDIR)target$(MAKEXT)
if((echo EXE=\>$(MAKEXE)) && for /f %I in ('findstr /s /m "\<main\>" *$(SRCEXT)') do @echo %~nI$(EXEEXT)\>>$(MAKEXE))
include $(MAKEXE)
else
error(Cannot find program main)
endif
#else // GNU
EXE=$(addsuffix $(EXEEXT), $(basename $(notdir $(shell grep -l --color=never "\bmain\b" $(SRC)))))
#endif

#ifdef _NMAKE
MAKOBJ=$(MAKDIR)object$(MAKEXT)
if((echo OBJ=\>$(MAKOBJ)) && for %I in ($(SRC)) do @echo $(OBJDIR)%~nI$(OBJEXT)\>>$(MAKOBJ))
include $(MAKOBJ)
else
error(Cannot name object files)
endif
#else // GNU
OBJ=$(patsubst $(SRCDIR)%$(SRCEXT), $(OBJDIR)%$(OBJEXT), $(SRC))
#endif

#ifdef _NMAKE
MAKDEP=$(MAKDIR)depend$(MAKEXT)
if((echo DEP=\>$(MAKDEP)) && for %I in ($(SRC)) do @echo $(DEPDIR)%~nI$(DEPEXT)\>>$(MAKDEP))
include $(MAKDEP)
endif
#else // GNU
DEP=$(patsubst $(SRCDIR)%$(SRCEXT), $(OBJDIR)%$(DEPEXT), $(SRC))
#endif

#ifdef _NMAKE
ALLDEP=$(MAKDIR)all$(MAKEXT)
if((echo. >$(ALLDEP)) && for %I in ($(DEP)) do @if exist "%I" @echo include %I >> $(ALLDEP))
include $(ALLDEP)
endif
#else // GNU
-include $(DEP)
#endif

//
// Compiler Options
//

#ifdef _MSC_VER

OBJEXT=.obj
DEPEXT=.dep
INLEXT=.inl
LIBEXT=.lib
PCHEXT=.pch
DBGEXT=.pdb

ifdef STD
add(CFLAGS, -std:$(STD))
endif

add(CFLAGS, -nologo -EHsc -permissive- -DNOMINMAX)

ifndef NDEBUG
add(WARN, -Z7 -W4 -D_DEBUG -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
add(LINK, -Z7)
endif

ifdef PCH
PCHHDR=$(SRCDIR)$(PCH)$(HDREXT)
PCHSRC=$(SRCDIR)$(PCH)$(SRCEXT)
PCHOBJ=$(OBJDIR)$(PCH)$(OBJEXT)
PCHOUT=$(OBJDIR)$(PCH)$(PCHEXT)
add(HEAD, -Yu$(PCH)$(HDREXT) -FI$(PCH)$(HDREXT) -Fp$(PCHOUT))
add(LINK, -Yu$(PCH)$(HDREXT))
$(PCHOBJ): $(PCHHDR); $(CXX) $(CFLAGS) $(WARN) -Yc$(PCH)$(HDREXT) -c $(PCHSRC) -Fo$(PCHOBJ) -Fp$(PCHOUT)
endif

CXXCMD=$(CXX) $(CFLAGS) $(WARN) $(HEAD) -c $< -Fo$(OBJDIR)
LNKCMD=$(CXX) $(CFLAGS) $(LINK) $(OBJ) -Fe$@
LNKDEP=$(PCHOBJ) $(OBJ) $(DEP)

#ifdef _NMAKE // make *.dep files
ifdef COMSPEC
{$(SRCDIR)}$(SRCEXT){$(DEPDIR)}$(DEPEXT):
	@echo $< : \> $@
	@set CMD=$(CXX) $(CFLAGS) -showIncludes -w -P -Fi"NUL" -c $<
	@for /F "tokens=1,2,3,*" %%A in ('%CMD% 2^>^&1') do @if not "%%D"=="" @echo "%%D" \>> $@
endif
#endif

#elif defined(__GNUC__) || defined(__llvm__) || defined(__clang__)

OBJEXT=.o
DEPEXT=.d
INLEXT=.i
LIBEXT=.a
PCHEXT=.gch
DBGEXT=.debug
EXPEXT=.exp
ILKEXT=.ilk

ifdef STD
add(CFLAGS, -std=$(STD))
endif

add(CFLAGS, -MP -MMD)
add(LINK, -rdynamic)

#if defined(__llvm__) || defined(__clang__)
add(CFLAGS, -stdlib=libc++)
add(LINK, -lc++ -lc++abi)
#endif

ifndef NDEBUG
add(WARN, -D_DEBUG -Wall -Wextra -Wpedantic -g)
endif

ifdef PCH
PCHHDR=$(SRCDIR)$(PCH)$(HDREXT)
PCHOUT=$(OBJDIR)$(PCH)$(PCHEXT)
add(HEAD, -include $(PCHHDR))
$(PCHOUT): $(PCHHDR); $(CXX) $(CFLAGS) $(WARN) -c $< -o $@
endif

CXXCMD=$(CXX) $(CFLAGS) $(WARN) $(HEAD) -c $< -o $@
LNKCMD=$(CXX) $(CFLAGS) $(LINK) $(OBJ) -o $@
LNKDEP=$(PCHOUT) $(OBJ)

#else
#error Cannot determine your compiler
#endif

//
// Build Targets
//

.SUFFIXES: $(SRCEXT) $(HDREXT) $(DEPEXT) $(MAKEXT) $(PCHEXT) $(OBJEXT) $(EXEEXT) $(LIBEXT) $(INLEXT) $(DBGEXT) $(EXPEXT) $(ILKEXT)

all: $(EXE)
help: ; $(SHOW) Readme
clean: ; $(REMOVE) $(OBJ) $(EXE) $(DEP) $(PCHOUT) $(PCHOBJ)
cflags: ; @echo $(CFLAGS) $(WARN) $(HEAD) > compile_flags.txt
ctags: ; ctags $(SRC) $(HDR)
info: build tool lang time shell;

$(EXE): $(LNKDEP); $(LNKCMD)
#ifdef _NMAKE
{$(SRCDIR)}$(SRCEXT){$(OBJDIR)}$(OBJEXT):: ; $(CXXCMD)
#else // GNU
$(OBJDIR)%$(OBJEXT): $(SRCDIR)%$(SRCEXT); $(CXXCMD)
#endif

//
// Environment
//

shell: ; @echo Command Shell $(CMD)

#ifdef __TIMESTAMP__
TIMESTAMP=__TIMESTAMP__
#else
TIMESTAMP=__DATE__ __TIME__
#endif

time: ; @echo $(TIMESTAMP)

#ifdef _MSC_VER
LANG=Visual C++ _MSVC_LANG
#elif defined(__cplusplus)
LANG=C++ __cplusplus
#elif defined(__STDC__)
LANG=C __STDC_VERSION__
#endif

lang: ; @echo $(LANG)

#ifdef _NMAKE
TOOL=NMake $(_NMAKE_VER)
#else
TOOL=GNU Make $(MAKE_VERSION)
#endif

tool: ; @echo $(TOOL)

#ifdef _MSC_VER
BUILD=MSVC _MSC_FULL_VER
#elif defined(__llmv__) || defined(__clang__)
BUILD=LLVM __clang_version__
#elif defined(__GNUC__)
BUILD=GCC __VERSION__
#endif

build: ; @echo $(BUILD)
