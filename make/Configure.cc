
// Macros
#ifdef _NMAKE
# define ifdef !ifdef
# define ifndef !ifndef
# define ifeq(x,y) !if #x == #y
# define ifneq(x,y) !if #x != #y
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
# define add(x, y) x += y
#endif

// System
ifeq($(OS),Windows_NT)
	OUT=.exe
	RM=del /f
	MKD=md
	DIR=\
	ENT=;
	ifndef WINVER
		WINVER=0xA000
	endif
	add(CFLAGS, -D_WIN32 -D_WIN32_WINNT=$(WINVER))
else // POSIX
	OUT=.out
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
.SUFFIXES: .cpp .hpp .mak $(OUT)
STD=c++17
MAKDIR=make$(DIR)
SRCDIR=src$(DIR)
OBJDIR=obj$(DIR)
ALLHDR=$(SRCDIR)*.hpp
ALLSRC=$(SRCDIR)*.cpp
PRE=$(SRCDIR)pre.hpp
BIN=$(SRCDIR)test.cpp

all: test$(OUT)

// Target
#ifdef _NMAKE
ifdef COMSPEC
	MAKHDR=$(MAKDIR)Header.mak
	!if ![echo HDR=>$(MAKHDR)\ ] 
		!if ![for %i in ($(ALLHDR)) do echo %i>>$(MAKHDR)\ ]
			!include $(MAKHDR)
		else
			error("No header")
		endif
	endif
	MAKSRC=$(MAKDIR)Source.mak
	!if ![echo SRC=>>$(MAKSRC)\ ]
		!if ![for %i in ($(ALLSRC)) do echo %i>>$(MAKSRC)\ ]
			!include $(MAKSRC)
		else
			error("No source")
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

#if 0 //#ifdef _MSC_VER

.SUFFIXES: .obj .dep. .inl .pch .lib .pdb .ilk
add(CFLAGS, /nologo /DNOMINMAX /EHsc /permissive-)
add(LDFLAGS, /nologo)
add(CCFLAGS, $(CFLAGS))

// Standard
ifdef STD
	add(CFLAGS, /std:%(STD))
endif

// Debug
ifndef NDEBUG
	add(CFLAGS, /Z7 /W4 /D_CRT_SECURE_NO_WARNINGS)
	add(LDFLAGS, /Z7)
endif

// Header
ifdef PRE
	PCH=$(PRE:.hpp=.pch)
	$(PCH): $(PRE); $(CXX) $(CFLAGS) /Yc$(PCH) /Fe$@ /c $<
	add(CCFLAGS, -Yu$(PCH) -FI$(PRE))
	add(LDFLAGS, -Yu$(PCH))
endif

// Outputs
OBJ=$(SRC:.cpp=.obj)
DEP=$(SRC:.cpp=.dep)

// Rules
test.exe: $(PCH) $(OBJ) $(DEP); $(CXX) $(LDFLAGS) $(OBJ) -Yu$(PCH) -Fe$@
#ifdef _NMAKE
{$(OBJDIR)}.obj{$(OBJDIR)}.cpp:; $(CXX) $(CCFLAGS) /c $<
#else
$(OBJDIR)%.obj: $(SRCDIR)%.cpp; $(CXX) $(CCFLAGS) /c $<
#endif
ifdef COMSPEC
.obj.dep:
	@echo $< : \> $@
	@set COMMAND=$(CXX) /std:$(STD) /permissive- /EHsc /DNOMINMAX /nologo /showIncludes /Zs /c $<
	@for /F "tokens=1,2,3,*" %%A in ('%COMMAND%') do @if not "%%D"=="" @echo "%%D" \>> $@
endif

#elif defined(__GNUC__) || defined(__llvm__) || defined(__clang__)

.SUFFIXES: .o .d .i .p .a
add(CFLAGS, -MP -MMD)
add(LDFLAGS, -rdynamic)
add(CXXFLAGS, $(CFLAGS))

// Standard
ifdef STD
	add(CFLAGS, -std=$(STD))
endif

// Debug
ifndef NDEBUG
	add(CFLAGS, -Wall -Wextra -Wpedantic -g)
endif

// Header
ifdef PRE
	PCH=$(PRE).gch
	$(PCH): $(PRE); $(CXX) $(CFLAGS) -c $< -o $@
	add(CXXFLAGS, -include $(PRE))
endif

// Outputs
#ifdef _NMAKE
// todo
#else // GNU
DEP=$(patsubst $(SRCDIR)%.cpp, $(OBJDIR)%.d, $(SRC))
OBJ=$(patsubst $(SRCDIR)%.cpp, $(OBJDIR)%.o, $(SRC))
#endif

// Rules
test$(OUT): $(OBJ); $(CXX) $(LDFLAGS) $(OBJ) -o $@
#ifdef _NMAKE
{$(OBJDIR)}.o{$(OBJDIR)}.cpp: ; $(CXX) $(CXXFLAGS) -c $<
#else // GNU
$(OBJDIR)%.o: $(SRCDIR)%.cpp $(PCH); $(CXX) $(CXXFLAGS) -c $< -o $@
#endif

#endif // Compile //

// Clean
clean: ; $(RM) test$(OUT) $(OBJ) $(DEP) $(PCH)

// Depend
#ifdef _NMAKE
ifdef COMSPEC
	MAKDEP=$(MAKDIR)Depend.mak
	!if ![(for %i in ($(DEP)) do @echo !include %i) > $(MAKDEP)]
		!include $(MAKDEP)
	endif
endif
#else // GNU
-include $(DEP)
#endif

// Extra
.SUFFIXES: .mk
include $(MAKDIR)Cppcheck.mk
include $(MAKDIR)PVS.mk

