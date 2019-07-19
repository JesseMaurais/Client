# Common configuration variables for the project

STD = c++17
SRC = test.cpp dir.cpp dll.cpp fifo.cpp file.cpp mem.cpp dbg.cpp env.cpp sig.cpp socket.cpp sys.cpp xdg.cpp
BIN = test

# Configurations for system, compiler, and make

include .make/Configure.mk

# Common make rules for the project

.SUFFIXES: .cpp .hpp .obj .o .d .i .log .xml .html .ilk .pdb

EXE = $(BIN)$(EXEEXT)

all: $(EXE)

clean:
	$(RM) $(EXE) $(OBJ) $(DEP) $(SRC:.cpp=.i) $(SRC:.cpp=.log) $(BIN).ilk $(BIN).pdb $(BIN).lib $(BIN).exp

$(EXE): $(OBJ)
	$(CXX) $(LDFLAGS) $(OUT)$(EXE) $(OBJ) $(LNK)

.cpp.o:
	$(CXX) $(CFLAGS) -c $<

.cpp.obj:
	$(CXX) $(CFLAGS) -c $<

# Additional features

include .make/Depends.mk
include .make/Cppcheck.mk
include .make/PVS.mk
