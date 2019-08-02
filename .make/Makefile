# 1 ".make/Makefile.cc"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 382 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 ".make/Makefile.cc" 2
# 24 ".make/Makefile.cc"
# 1 ".make/LLVM.mk" 1

# 1 ".make/GCC.mk" 1




CFLAGS += -MP -MMD

CFLAGS += -g

LDFLAGS += -rdynamic

WARN=-Wall -Wextra -Wpedantic
OUT=-o
# 3 ".make/LLVM.mk" 2
ANAL=--analyze -Xanalyzer -analyzer-output=html
# 25 ".make/Makefile.cc" 2
# 48 ".make/Makefile.cc"
# 1 ".make/POSIX.mk" 1




CFLAGS += -D_POSIX_C_SOURCE
LNK += -ldl -lrt -lpthread
# 49 ".make/Makefile.cc" 2




ifdef SHELL
include .make/SH.mk
else
ifdef COMSPEC
include .make/CMD.mk
else
$(error "Cannot determine your system commands.")
endif
endif




# 1 ".make/../Tools.mk" 1



STD=c++17
SRC=test.cpp dbg.cpp dir.cpp dll.cpp env.cpp err.cpp fifo.cpp file.cpp shm.cpp sig.cpp socket.cpp sys.cpp xdg.cpp
BIN=test



.SUFFIXES: .cpp .hpp .obj .d .inl .log .xml .html .ilk .pdb .db

OBJ=$(SRC:.cpp=.o)
EXE=$(BIN)
CFLAGS += -std=$(STD)

all: $(EXE)

clean: ; $(RM) $(EXE) $(OBJ) $(BIN).ilk $(BIN).pdb $(BIN).lib $(BIN).exp $(SRC:.cpp=.log) $(SRC:.cpp=.i) $(SRC:.cpp=.d)

$(EXE): $(OBJ); $(CXX) $(LDFLAGS) $(OUT)$@ $(OBJ) $(LNK)

.cpp.o: ; $(CXX) $(CFLAGS) -c $<

-include $(SRC:.cpp=.d)
# 66 ".make/Makefile.cc" 2

