# Common configuration variables for the project
STD = c++17
SRC = test.cpp file.cpp env.cpp sys.cpp sig.cpp dbg.cpp xdg.cpp
BIN = test
LOG = $(SRC:.cpp=.log)
INL = $(SRC:.cpp=.i)
TSK = .make/Task.htm

# Configurations for system, compiler, and make
include .make/Configure.mk

.SUFFIXES: .cpp .hpp .obj .o .d .i .log

EXE = $(BIN)$(EXEEXT)

all: $(EXE)

task: $(TSK)

clean:
	$(RM) $(EXE) $(OBJ) $(DEP) $(LOG) $(INL)

$(EXE): $(OBJ)
	$(CXX) $(CFLAGS) $(COUT)$(EXE) $(OBJ)

$(TSK): $(LOG)
	plog-converter -a 'GA:1,2' -t html $(LOG) -o $@

.cpp.log:
	$(CXX) $< -E -o $*.i
	pvs-studio --cfg .make/PVS.cfg --i-file $*.i --output-file $@ --source-file $<

.cpp.o:
	$(CXX) $(CFLAGS) -c $<

.cpp.obj:
	$(CXX) $(CFLAGS) -c $<

# Generated dependencies
include .make/Depends.mk
