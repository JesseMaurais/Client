# Common configuration variables for the project

STD = c++17
SRC = test.cpp dir.cpp dll.cpp fifo.cpp file.cpp dbg.cpp env.cpp shm.cpp sig.cpp socket.cpp sys.cpp xdg.cpp
BIN = test

# Configurations for system, compiler, and make

include .make/Configure.mk

# Common make rules for the project

.SUFFIXES: .cpp .hpp .obj .o .d .i .log .xml .html .ilk .pdb .db

all: $(BIN)$(EXE)

clean:
	$(RM) $(BIN)$(EXE) $(SRC:.cpp=.obj) $(SRC:.cpp=.o) $(SRC:.cpp=.d) $(SRC:.cpp=.i) $(SRC:.cpp=.db) $(SRC:.cpp=.log) $(BIN).ilk $(BIN).pdb $(BIN).lib $(BIN).exp

$(BIN)$(EXE): $(OBJ)
	$(CXX) $(LDFLAGS) $(OUT)$(BIN)$(EXE) $(OBJ) $(LNK)

.cpp.o:
	$(CXX) $(CFLAGS) -c $<

.cpp.obj:
	$(CXX) $(CFLAGS) -c $<

# Additional features

include .make/Depends.mk
include .make/Cppcheck.mk
include .make/PVS.mk
