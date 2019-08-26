#line 1 ".make/Makefile.cc"













#line 15 ".make/Makefile.cc"



!ifdef SHELL
RM=rm -f
MD=mkdir -p
!else
!ifdef COMSPEC
RM=del -f
MD=md
!else
!error "Cannot determine your system commands."
!endif 
!endif 



STD=c++17
PCH=pre.hpp
SRC=cpu.cpp dbg.cpp dev.cpp dir.cpp dll.cpp env.cpp err.cpp fifo.cpp file.cpp ini.cpp int.cpp opt.cpp shm.cpp sig.cpp socket.cpp sys.cpp usr.cpp
BIN=test.cpp docy.cpp
ALL=$(SRC) $(BIN)

.SUFFIXES: .cpp .hpp .o .d .gch .obj .pdb .pch .lib .exp .ilk .log .i .db




CFLAGS=$(CFLAGS) -D_WIN32
EXE=$(BIN:.cpp=.exe)






#line 52 ".make/Makefile.cc"



all: $(EXE)

clean: ; $(RM) $(EXE) *.o *.d *.gch *.obj *.pdb *.pch *.lib *.exp *.ilk *.log *.i





CFLAGS=$(CFLAGS) -nologo -std:$(STD) -W4 -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -EHsc -permissive-
LDFLAGS=$(LDFLAGS) -nologo
!ifndef NDEBUG
CFLAGS=$(CFLAGS) -Z7
LDFLAGS=$(LDFLAGS) -Z7
!endif

OBJ=$(SRC:.cpp=.obj)
DEP=$(PCH:.hpp=.pch) $(OBJ)

test.exe: $(DEP) test.obj; $(CXX) $(LDFLAGS) $(OBJ) test.obj -Fe$@
docy.exe: $(DEP) docy.obj; $(CXX) $(LDFLAGS) $(OBJ) docy.obj -Fe$@
.cpp.obj: ; $(CXX) $(CFLAGS) -Yu$(PCH) -FI$(PCH) -c $<
$(PCH:.hpp=.pch): $(PCH); $(CXX) $(CFLAGS) -Yc$(PCH) -FI$(PCH) -c $(SRC) $(BIN)






















#line 100 ".make/Makefile.cc"



include .make/Cppcheck.mk
include .make/PVS.mk
