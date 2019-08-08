# 1 ".make/Makefile.cc"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 382 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 ".make/Makefile.cc" 2
# 18 ".make/Makefile.cc"
ifdef SHELL
RM=rm -f
MD=mkdir -p
else
ifdef COMSPEC
RM=del /f
MD=md
else
$(error "Cannot determine your system commands.")
endif
endif



STD=c++17
SRC=test.cpp cpu.cpp dbg.cpp dir.cpp dll.cpp env.cpp err.cpp fifo.cpp file.cpp opt.cpp shm.cpp sig.cpp socket.cpp sys.cpp usr.cpp
PCH=pre.hpp
BIN=test

.SUFFIXES: .cpp .hpp .o .d .gch .obj .pdb .pch .lib .exp .ilk .log .i .db







CFLAGS += -D_POSIX_C_SOURCE
LDFLAGS += -ldl -lrt -lpthread
EXE=$(BIN)




all: $(EXE)

clean: ; $(RM) $(EXE) *.o *.d *.gch *.obj *.pdb *.pch *.lib *.exp *.ilk *.log *.i
# 74 ".make/Makefile.cc"
CFLAGS += -std=$(STD) -Wall -Wextra -Wpedantic -MP -MMD
ifndef NDEBUG
CFLAGS += -g
endif
LDFLAGS += -rdynamic

OBJ=$(SRC:.cpp=.o)

$(EXE): $(PCH).gch $(OBJ); $(CXX) $(LDFLAGS) $(OBJ) -o $@
.cpp.o: ; $(CXX) $(CFLAGS) -include $(PCH) -c $<
$(PCH).gch: $(PCH); $(CXX) $(CFLAGS) -c $<


-include $(SRC:.cpp=.d)







include .make/Cppcheck.mk
include .make/PVS.mk
