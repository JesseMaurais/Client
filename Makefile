MAKEFILES=Tools.mk

# Designed to work with both NMake and GNU make.

SRC=test.cpp file.cpp env.cpp sys.cpp dbg.cpp xdg.cpp

all: $(SRC)
	$(CXX) $(CFLAGS) $(SRC)

