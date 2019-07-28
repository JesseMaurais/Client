#pragma // Use the preprocessor to get configurations

#include "os.hpp"
#include "cc.hpp"

#pragma // Common configuration variables for the project

STD=c++17
SRC=test.cpp dir.cpp dll.cpp fifo.cpp file.cpp dbg.cpp env.cpp shm.cpp sig.cpp socket.cpp sys.cpp xdg.cpp
BIN=test

#pragma // Configurations for system, compiler, and make

include .make/Configure.mk

#pragma // Common make rules for the project

.SUFFIXES: .cpp .hpp .obj .d .inl .log .xml .html .ilk .pdb .db

OBJ=$(SRC:.cpp=.obj)

ALL: $(BIN)$(EXE)

clean: ; $(RM) $(BIN)$(EXE) $(OBJ) $(BIN).ilk $(BIN).pdb $(BIN).lib $(BIN).exp $(SRC:.cpp=.log) $(SRC:.cpp=.i) $(SRC:.cpp=.d)

$(BIN)$(EXE): $(OBJ); $(CXX) $(LDFLAGS) $(OUT)$@ $(OBJ) $(LNK)

.cpp.obj: ; $(CXX) $(CFLAGS) -c $< $(OUT)$@

#pragma // Additional features

include .make/Depends.mk
include .make/Cppcheck.mk
include .make/PVS.mk
