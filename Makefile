STD = c++17

include .make/Configure.mk

.SUFFIXES: .cpp .hpp .obj

SRC = test.cpp file.cpp env.cpp sys.cpp sig.cpp dbg.cpp xdg.cpp
OBJ = $(SRC:.cpp=.obj)
BIN = test$(EXEEXT)

all: $(BIN)

clean:
	$(DEL) $(OBJ) $(BIN)

$(BIN): $(OBJ) 
	$(CXX) $(CFLAGS) $(COUT)$(BIN) $(OBJ)

.cpp.obj:
	$(CXX) $(CFLAGS) $(COUT)$@ -c $<

