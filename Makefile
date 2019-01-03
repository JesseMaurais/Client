# Designed to work with both NMake and GNU make.

# \
!ifndef 0 
include Tools.mk
# \
!endif

SRC=test.cpp file.cpp env.cpp sys.cpp dbg.cpp xdg.cpp
BIN=test$(EXE)

clean:
	$(RM) *$(OBJ) $(BIN)

all: $(BIN)

$(BIN): $(SRC)
	$(CXX) $(CFLAGS) $(OUT)$@ $?

