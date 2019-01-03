# Designed to work with both NMake and GNU make.

# \
!ifndef 0 # 
include Tools.gnu
# \
!endif

SRC=test.cpp file.cpp env.cpp sys.cpp dbg.cpp xdg.cpp

all: $(SRC)
	$(CXX) $(CFLAGS) $(SRC)

