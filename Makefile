# Common configuration variables for the project

STD = c++17
SRC = test.cpp file.cpp mem.cpp dbg.cpp env.cpp sig.cpp sys.cpp xdg.cpp
BIN = test

LOG = $(SRC:.cpp=.log)
INL = $(SRC:.cpp=.i)
CHK = .make/CppCheck.xml
PVS = .make/PVS.htm

# Configurations for system, compiler, and make

include .make/Configure.mk

# Common make rules for the project

.SUFFIXES: .cpp .hpp .obj .o .d .i .log .ilk .pdb

EXE = $(BIN)$(EXEEXT)

all: $(EXE)

cppcheck: $(CHK)

pvscheck: $(PVS)

clean:
	$(RM) $(EXE) $(OBJ) $(DEP) $(INL) $(LOG) $(BIN).ilk $(BIN).pdb

$(EXE): $(OBJ)
	$(CXX) $(CFLAGS) $(OUT)$(EXE) $(OBJ)

$(CHK): $(SRC)
	cppcheck --force --quiet --enable=all $(SRC)

$(PVS): $(LOG)
	plog-converter -a 'GA:1,2;64:1;OP:1,2,3;CS:1;MISRA:1,2' -t html $(LOG) -o $@

.cpp.log:
	$(CXX) $< -E > $*.i
	pvs-studio --cfg .make/PVS.cfg --i-file $*.i --output-file $@ --source-file $<

.cpp.o:
	$(CXX) $(CFLAGS) -c $<

.cpp.obj:
	$(CXX) $(CFLAGS) -c $<

# Generated dependencies

include .make/Depends.mk
