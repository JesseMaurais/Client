# 1 ".make/Makefile.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 382 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 ".make/Makefile.c" 2


# 1 ".make/../os.hpp" 1
# 4 ".make/Makefile.c" 2
# 1 ".make/../cc.hpp" 1
# 5 ".make/Makefile.c" 2



STD=c++17
SRC=test.cpp dbg.cpp dir.cpp dll.cpp env.cpp err.cpp fifo.cpp file.cpp shm.cpp sig.cpp socket.cpp sys.cpp xdg.cpp
BIN=test



include .make/Configure.mk



.SUFFIXES: .cpp .hpp .obj .d .inl .log .xml .html .ilk .pdb .db

OBJ=$(SRC:.cpp=.obj)

ALL: $(BIN)$(EXE)

clean: ; $(RM) $(BIN)$(EXE) $(OBJ) $(BIN).ilk $(BIN).pdb $(BIN).lib $(BIN).exp $(SRC:.cpp=.log) $(SRC:.cpp=.i) $(SRC:.cpp=.d)

$(BIN)$(EXE): $(OBJ); $(CXX) $(LDFLAGS) $(OUT)$@ $(OBJ) $(LNK)

.cpp.obj: ; $(CXX) $(CFLAGS) -c $< $(OUT)$@



include .make/Depends.mk
include .make/Cppcheck.mk
include .make/PVS.mk
