
STD = c++17

include .make/Configure.mk

.SUFFIXES: $(EXEEXT) $(OBJEXT) .cpp .hpp

SRC = test.cpp file.cpp env.cpp sys.cpp sig.cpp dbg.cpp xdg.cpp
OBJ = $(SRC:.cpp=$(OBJEXT))
BIN = test$(EXEEXT)

all: $(BIN)

clean:
	$(RM) $(OBJ) $(BIN)

$(BIN): $(OBJ) 
	$(CXX) $(OBJ) -o $@

%$(OBJEXT): %.cpp
	$(CXX) $(CFLAGS) -o $@ -c $<

%$(OBJEXT): %.cpp %.hpp
	$(CXX) $(CFLAGS) -o $@ -c $<

