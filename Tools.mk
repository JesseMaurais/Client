
// Project Variables

STD=c++17
SRC=test.cpp dbg.cpp dir.cpp dll.cpp env.cpp err.cpp fifo.cpp file.cpp shm.cpp sig.cpp socket.cpp sys.cpp xdg.cpp
BIN=test

.SUFFIXES: .cpp .hpp .obj .d .inl .log .xml .html .ilk .pdb .db

// C++ Compiler

#if defined(__GNUC__) || defined(__llvm__) || defined(__clang__)
cat(CFLAGS, -std=$(STD) -Wall -Wextra -Wpedantic -MP -MMD)
# ifndef NDEBUG
cat(CFLAGS, -g)
# endif
# ifndef _NMAKE
#  define DEPENDS -include $(SRC:.cpp=.d)
# endif
cat(LDFLAGS, -rdynamic -o $@)
OBJ=$(SRC:.cpp=.o)
#elif defined(_MSC_VER)
cat(CFLAGS, -nologo -std:$(STD) -W4 -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -EHsc -permissive-)
# ifndef NDEBUG
cat(CFLAGS, -Zi)
# endif
cat(LDFLAGS, -Fa$@)
OBJ=$(SRC:.cpp=.obj)
#else
# error "Cannot determine your compiler."
#endif

// Operating Sytem

#if defined(_WIN32)
cat(CFLAGS, -D_WIN32)
EXE=$(BIN).exe
#else
cat(CFLAGS, -D_POSIX_C_SOURCE)
cat(LDFLAGS, -ldl -lrt -lpthread)
EXE=$(BIN)
#endif

// Make Rules

all: $(EXE)

clean: ; $(RM) $(EXE) $(OBJ) *.d *.pdb *.lib *.exp *.ilk *.log *.i

$(EXE): $(OBJ); $(CXX) $(LDFLAGS) $(OBJ)

.cpp.o: ; $(CXX) $(CFLAGS) -c $<

.cpp.obj: ; $(CXX) $(CFLAGS) -c $<

#ifdef DEPENDS
DEPENDS
#endif
