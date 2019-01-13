# Common configuration variables for the project
STD = c++17
SRC = test.cpp file.cpp env.cpp sys.cpp sig.cpp dbg.cpp xdg.cpp
BIN = test

# Configurations for system, compiler, and make
include .make/Configure.mk

EXE = $(BIN)$(EXEEXT)

all: $(EXE)

clean:
	$(RM) $(EXE) $(OBJ) $(DEP)

$(EXE): $(OBJ)
	$(CXX) $(CFLAGS) $(COUT)$(EXE) $(OBJ)

.cpp.o:
	$(CXX) $(CFLAGS) -D_XOPEN_SOURCE -c $<

.cpp.obj:
	$(CXX) $(CFLAGS) -c $<

# Generated dependencies
include .make/Depends.mk
