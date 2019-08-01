#line 1 ".make/Makefile.cc"


#line 1 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"









#line 11 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"





#line 17 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"





#line 23 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"





#line 29 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"





#line 35 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"





#line 41 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"





#line 47 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"





#line 53 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"





#line 59 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"





#line 65 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"





#line 71 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"





#line 77 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"





#line 83 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"





#line 89 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"










#line 100 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"





#line 106 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"





#line 112 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"





#line 118 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"





#line 124 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"





#line 130 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"

#line 132 "c:\\users\\jessem\\documents\\github\\client\\.make\\../os.hpp"
#line 4 ".make/Makefile.cc"
#line 1 "c:\\users\\jessem\\documents\\github\\client\\.make\\../cc.hpp"









#line 11 "c:\\users\\jessem\\documents\\github\\client\\.make\\../cc.hpp"





#line 17 "c:\\users\\jessem\\documents\\github\\client\\.make\\../cc.hpp"





#line 23 "c:\\users\\jessem\\documents\\github\\client\\.make\\../cc.hpp"





#line 29 "c:\\users\\jessem\\documents\\github\\client\\.make\\../cc.hpp"





#line 35 "c:\\users\\jessem\\documents\\github\\client\\.make\\../cc.hpp"





#line 41 "c:\\users\\jessem\\documents\\github\\client\\.make\\../cc.hpp"





#line 47 "c:\\users\\jessem\\documents\\github\\client\\.make\\../cc.hpp"





#line 53 "c:\\users\\jessem\\documents\\github\\client\\.make\\../cc.hpp"





#line 59 "c:\\users\\jessem\\documents\\github\\client\\.make\\../cc.hpp"


#line 62 "c:\\users\\jessem\\documents\\github\\client\\.make\\../cc.hpp"
#line 5 ".make/Makefile.cc"



STD=c++17
SRC=test.cpp dbg.cpp dir.cpp dll.cpp env.cpp err.cpp fifo.cpp file.cpp shm.cpp sig.cpp socket.cpp sys.cpp xdg.cpp
BIN=test



include .make/Configure.mk



.SUFFIXES: .cpp .hpp .obj .d .inl .log .xml .html .ilk .pdb .db

OBJ=$(SRC:.cpp=.obj)

ALL: $(BIN)$(EXE)

clean: ; $(RM) $(BIN)$(EXE) $(OBJ) $(BIN).ilk $(BIN).pdb $(BIN).lib $(BIN).exp $(SRC:.cpp=.log) $(SRC:.cpp=.i) $(SRC:.cpp=.d)

$(BIN)$(EXE): $(OBJ); $(CXX) $(LDFLAGS) $(OUT)$@ $(OBJ) $(LNK)

.cpp.obj: ; $(CXX) $(CFLAGS) -c $< $(OUT)$@



include .make/Depends.mk
include .make/Cppcheck.mk
include .make/PVS.mk
