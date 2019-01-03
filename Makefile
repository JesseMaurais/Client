# Designed to work with both NMake and GNU make.

# \
!ifdef 0 
include Tools.mk
# \
!endif

SRC=test.cpp file.cpp env.cpp sys.cpp dbg.cpp xdg.cpp
BIN=test$(EXE)

all: $(BIN)

clean:
	$(RM) *$(OBJ) $(BIN)

$(BIN): $(SRC)
	$(CXX) $(CFLAGS) $(OUT)$@ $?

