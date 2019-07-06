# Common configuration variables for the project

BUGMSG = Known GCC bug up to version 8.2 causes internal compiler error

STD = c++17
SRC = test.cpp dl.cpp fifo.cpp file.cpp mem.cpp dbg.cpp env.cpp sig.cpp socket.cpp sys.cpp xdg.cpp
BIN = test

LOG = $(SRC:.cpp=.log)
INL = $(SRC:.cpp=.i)
CHK = .make/CppCheck.htm
PVS = .make/PVS.htm

# Configurations for system, compiler, and make

include .make/Configure.mk

# Common make rules for the project

.SUFFIXES: .cpp .hpp .obj .o .d .i .log .xml .htm .ilk .pdb

EXE = $(BIN)$(EXEEXT)

all: $(EXE)

cppcheck: $(CHK)

pvs: $(PVS)

clean:
	$(RM) $(EXE) $(OBJ) $(DEP) $(INL) $(LOG) $(BIN).ilk $(BIN).pdb $(BIN).lib $(BIN).exp

$(EXE): $(OBJ)
	$(CXX) $(LDFLAGS) $(OUT)$(EXE) $(OBJ) $(LNK)

$(CHK): $(SRC)
	cppcheck --force --quiet --verbose --enable=all $(SRC)
	#cppcheck-htmlreport --report-dir=.make --source-dir=. --file=$*.xml 

$(PVS): $(SRC) $(LOG)
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
