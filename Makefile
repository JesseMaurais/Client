STD = c++17

include .make/Configure.mk

SRC = test.cpp file.cpp env.cpp sys.cpp sig.cpp dbg.cpp xdg.cpp
OBJ = $(SRC:.cpp=.o)
DEP = $(SRC:.cpp=.d)
BIN = test$(EXE)

all: $(BIN)

clean:
	$(RM) $(BIN) $(OBJ) $(DEP)

$(BIN): $(OBJ) 
	$(CXX) $(CFLAGS) $(COUT)$(BIN) $(OBJ)

.cpp.o:
	$(CXX) $(CFLAGS) $(COUT)$@ -c $<

-include $(DEP)

