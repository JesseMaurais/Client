
# Designed to work with both NMake and GNU make.
# You may need to specify CXX on the command line.

SRC=sys.cpp file.cpp env.cpp main.cpp dbg.cpp xdg.cpp

all: $(SRC)
	$(CXX) -std=c++17 -Wall $(CFLAGS) $(SRC)

