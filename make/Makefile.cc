
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
else
ifdef COMSPEC
RM=del /f
else
err("Cannot determine your system commands.")
endif // COMSPEC
endif // SHELL

// Project Variables

STD=c++17
SRC=test.cpp dbg.cpp dir.cpp dll.cpp env.cpp err.cpp fifo.cpp file.cpp shm.cpp sig.cpp socket.cpp sys.cpp xdg.cpp
BIN=test

.SUFFIXES: .cpp .hpp .o .d .obj .pdb .lib .exp .ilk .log .i .db

// Operating Sytem

#ifdef _WIN32
add(CFLAGS, -D_WIN32)
EXE=$(BIN).exe
#else
add(CFLAGS, -D_POSIX_C_SOURCE)
add(LDFLAGS, -ldl -lrt -lpthread)
EXE=$(BIN)
#endif

// Make Targets

all: $(EXE)

clean: ; $(RM) $(EXE) *.o *.d *.obj *.pdb *.lib *.exp *.ilk *.log *.i

// Compiler Options

#ifdef _MSC_VER

add(CFLAGS, -nologo -std:$(STD) -W4 -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -EHsc -permissive-)
ifndef NDEBUG
add(CFLAGS, -Zi)
endif
add(LDFLAGS, -nologo)

OBJ=$(SRC:.cpp=.obj)

$(EXE): $(OBJ); $(CXX) $(LDFLAGS) $(OBJ) -Fe$@
.cpp.obj: ; $(CXX) $(CFLAGS) -c $<

#elif defined(__GNUC__) || defined(__llvm__) || defined(__clang__)

add(CFLAGS, -std=$(STD) -Wall -Wextra -Wpedantic -MP -MMD)
ifndef NDEBUG
add(CFLAGS, -g)
endif
add(LDFLAGS, -rdynamic)

OBJ=$(SRC:.cpp=.o)

$(EXE): $(OBJ); $(CXX) $(LDFLAGS) $(OBJ) -o $@
.cpp.o: ; $(CXX) $(CFLAGS) -c $<

# ifndef _NMAKE
-include $(SRC:.cpp=.d)
# endif
#else
# error "Cannot determine your compiler."
#endif

// Static Code Analysis

include make/Cppcheck.mk
include make/PVS.mk
