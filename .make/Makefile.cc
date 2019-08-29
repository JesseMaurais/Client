
// Make Version

#ifdef _NMAKE
# define ifdef !ifdef
# define ifndef !ifndef
# define else !else
# define endif !endif
# define err(x) !error x
# define add(x, y) x=$(x) y
#else // GNU Make
# define err(x) $(error x)
# define add(x, y) x += y
#endif

// System Commands

ifdef SHELL
RM=rm -f
MD=mkdir -p
else
ifdef COMSPEC
RM=del /f
MD=md
else
err("Cannot determine your system commands.")
endif // COMSPEC
endif // SHELL

// Project Variables

STD=c++17
PCH=pre.hpp
SRC=cpu.cpp dbg.cpp dev.cpp dir.cpp dll.cpp env.cpp err.cpp fifo.cpp file.cpp ini.cpp int.cpp opt.cpp shm.cpp sig.cpp socket.cpp sys.cpp usr.cpp
BIN=test.cpp docy.cpp
ALL=$(SRC) $(BIN)

.SUFFIXES: .cpp .hpp .o .a .d .gch .obj .lib .dep .pch .pdb .exp .ilk .log .i .db

// Operating Sytem

#ifdef _WIN32
add(CFLAGS, -D_WIN32_WINNT=0x0A00)
EXE=$(BIN:.cpp=.exe)
#define exe(x) x.exe
#else
add(CFLAGS, -D_XOPEN_SOURCE=600)
add(LDFLAGS, -ldl -lrt -lpthread)
EXE=$(BIN:.cpp=)
#define exe(x) x
#endif

// Make Targets

all: $(EXE)

clean: ; $(RM) $(EXE) *.o *.a *.d *.gch *.obj *.lib *.dep *.pch *.pdb *.exp *.ilk *.log *.i

// Compiler Options

#ifdef _MSC_VER

add(CFLAGS, -nologo -std:$(STD) -W4 -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -EHsc -permissive-)
add(LDFLAGS, -nologo)
ifndef NDEBUG
add(CFLAGS, -Z7)
add(LDFLAGS, -Z7)
endif

OBJ=$(SRC:.cpp=.obj) $(PCH:.hpp=.obj) 
DEP=$(ALL:.cpp=.dep) $(PCH:.hpp=.pch) $(OBJ)

exe(test): $(DEP) test.obj; $(CXX) $(LDFLAGS) $(OBJ) test.obj -Fe$@
exe(docy): $(DEP) docy.obj; $(CXX) $(LDFLAGS) $(OBJ) docy.obj -Fe$@
.cpp.obj: ; $(CXX) $(CFLAGS) -Yu$(PCH) -FI$(PCH) -c $<
$(PCH:.hpp=.pch): $(PCH); $(CXX) $(CFLAGS) -Yc$(PCH) -c $(PCH:.hpp=.cpp)

#ifdef _NMAKE
ifdef COMSPEC
.cpp.dep:
	@echo $(<:.cpp=.obj): \> $@
	@set COMMAND=$(CXX) -std:$(STD) -permissive- -EHsc -DNOMINMAX -nologo -showIncludes -Zs -c $<
	@for /F "tokens=1,2,3,*" %%A in ('%COMMAND%') do @if not "%%D"=="" @echo "%%D" \>> $@
endif
#endif

#elif defined(__GNUC__) || defined(__llvm__) || defined(__clang__)

add(CFLAGS, -std=$(STD) -Wall -Wextra -Wpedantic -MP -MMD)
ifndef NDEBUG
add(CFLAGS, -g)
endif
add(LDFLAGS, -rdynamic)

OBJ=$(SRC:.cpp=.o)
DEP=$(PCH).gch $(OBJ)

exe(test): $(DEP) test.o; $(CXX) $(LDFLAGS) $(OBJ) test.o -o $@
exe(docy): $(DEP) docy.o; $(CXX) $(LDFLAGS) $(OBJ) docy.o -o $@
.cpp.o: ; $(CXX) $(CFLAGS) -include $(PCH) -c $<
$(PCH).gch: $(PCH); $(CXX) $(CFLAGS) -c $<

#else
# error "Cannot determine your compiler."
#endif

// Auto-dependency resolution

#ifdef _NMAKE
ifdef COMSPEC
!if ![(for %i in (*.d) do @echo !include %i) > Depend.mk]
!include Depend.mk
endif
endif
#else
-include $(ALL:.cpp=.d)
#endif

// Static Code Analysis

include .make/Cppcheck.mk
include .make/PVS.mk
