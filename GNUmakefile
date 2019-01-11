include .make/GNU.mk

OBJ = $(SRC:.cpp=.o)
DEP = $(SRC:.cpp=.d)

all: $(BIN)

clean:
	$(RM) $(BIN) $(OBJ) $(DEP)

$(BIN): $(OBJ) 
	$(CXX) $(CFLAGS) $(COUT)$(BIN) $(OBJ)

.cpp.o:
	$(CXX) $(CFLAGS) -c $<

-include $(DEP)

