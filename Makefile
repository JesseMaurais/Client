include .make/NMake.mk

OBJ = $(SRC:.cpp=.obj)

all: $(BIN).exe

clean:
	$(RM) $(BIN) $(OBJ)

$(BIN).exe: $(OBJ) 
	$(CXX) $(CFLAGS) $(COUT)$@ $<

.cpp.obj:
	$(CXX) $(CFLAGS) $(COUT)$@ -c $<

