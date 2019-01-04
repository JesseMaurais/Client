# Designed to work with both NMake and GNU make.

include .make/Rules.mk

SRC = test.cpp file.cpp env.cpp sys.cpp dbg.cpp xdg.cpp
BIN = test$(EXE)

all: $(BIN)

clean:
	$(RM) *$(OBJ) $(BIN)

$(BIN): $(SRC)
	$(CXX) $(CFLAGS) $(OUT) $@ $?

