
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

all: $(OBJDIR) test$(OUT)
$(OBJDIR): ; $(MKD) $(OBJDIR)

// Source
#ifdef _NMAKE
ifdef COMSPEC
	MAKHDR=$(MAKDIR)Header.mak
	!if ![echo HDR=>$(MAKHDR)\ ] 
		!if ![for %i in ($(ALLHDR)) do echo %i>>$(MAKHDR)\ ]
			!include $(MAKHDR)
		endif
	endif
	MAKSRC=$(MAKDIR)Source.mak
	!if ![echo SRC=>>$(MAKSRC)\ ]
		!if ![for %i in ($(ALLSRC)) do echo %i>>$(MAKSRC)\ ]
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

.SUFFIXES: .obj .dep. .inl .pch .lib .pdb .ilk
add(CFLAGS, /nologo /DNOMINMAX /EHsc /permissive-)
add(LDFLAGS, /nologo)
add(CXXFLAGS, $(CFLAGS))

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
	$(PCH): $(PRE:.hpp=.cpp); $(CXX) $(CFLAGS) /Yc$(PCH) /Fe$@ /c $<
	add(CCFLAGS, -Yu$(PCH) -FI$(PRE))
	add(LDFLAGS, -Yu$(PCH))
endif

// Outputs
#ifdef _NMAKE
ifdef COMSPEC
	MAKDEP=$(MAKDIR)Depend.mak
	!if ![echo DEP=>$(MAKDEP)\ ] 
		!if ![for %I in ($(ALLSRC)) do echo $(OBJDIR)%~nI.dep>>$(MAKDEP)\ ]
			!include $(MAKDEP)
		endif
	endif
	MAKOBJ=$(MAKDIR)Object.mak
	!if ![echo OBJ=>>$(MAKOBJ)\ ]
		!if ![for %I in ($(ALLSRC)) do echo $(OBJDIR)%~nI.obj>>$(MAKOBJ)\ ]
			!include $(MAKOBJ)
		endif
	endif
endif
#else // GNU
DEP=$(patsubst $(SRCDIR)%.cpp, $(OBJDIR)%.dep, $(SRC))
OBJ=$(patsubst $(SRCDIR)%.cpp, $(OBJDIR)%.obj, $(SRC))
#endif

// Rules
test$(OUT): $(PCH) $(OBJ) $(DEP); $(CXX) $(LDFLAGS) $(OBJ) -Yu$(PCH) -Fe$@
#ifdef _NMAKE
{$(OBJDIR)}.obj{$(OBJDIR)}.cpp:; $(CXX) $(CXXFLAGS) /c $<
#else
$(OBJDIR)%.obj: $(SRCDIR)%.cpp; $(CXX) $(CXXFLAGS) /c $<
#endif
ifdef COMSPEC
.obj.dep:
	@echo $< : \> $@
	@set CMD=$(CXX) $(CFLAGS) /showIncludes /Zs /c $<
	@for /F "tokens=1,2,3,*" %%A in ('%CMD%') do @if not "%%D"=="" @echo "%%D" \>> $@
endif

#elif defined(__GNUC__) || defined(__llvm__) || defined(__clang__)

.SUFFIXES: .o .d .i .a .gch
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
ifdef COMSPEC
	MAKDEP=$(MAKDIR)Depend.mak
	!if ![echo DEP=>$(MAKDEP)\ ] 
		!if ![for %I in ($(ALLSRC)) do echo $(OBJDIR)%~nI.d>>$(MAKDEP)\ ]
			!include $(MAKDEP)
		endif
	endif
	MAKOBJ=$(MAKDIR)Object.mak
	!if ![echo OBJ=>>$(MAKOBJ)\ ]
		!if ![for %I in ($(ALLSRC)) do echo $(OBJDIR)%~nI.o>>$(MAKOBJ)\ ]
			!include $(MAKOBJ)
		endif
	endif
endif
#else // GNU
DEP=$(patsubst $(SRCDIR)%.cpp, $(OBJDIR)%.d, $(SRC))
OBJ=$(patsubst $(SRCDIR)%.cpp, $(OBJDIR)%.o, $(SRC))
#endif

// Rules
test$(OUT): $(PCH) $(OBJ); $(CXX) $(LDFLAGS) $(OBJ) -o $@
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
	MAKALLDEP=$(MAKDIR)All.mak
	!if ![(for %i in ($(DEP)) do @echo !include %i) > $(MAKALLDEP)]
		!include $(MAKALLDEP)
	endif
endif
#else // GNU
-include $(DEP)
#endif

// Extra
.SUFFIXES: .mk
include $(MAKDIR)Cppcheck.mk
include $(MAKDIR)PVS.mk

