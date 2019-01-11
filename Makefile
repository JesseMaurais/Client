include .make/NMake.mk

OBJ = $(SRC:.cpp=.obj)
EXE = $(BIN).exe

all: $(EXE)

clean:
	$(RM) $(EXE) $(OBJ)

$(EXE): $(OBJ)
	$(CXX) $(CFLAGS) $(COUT)$@ $**

.cpp.obj:
	$(CXX) $(CFLAGS) -c $<

