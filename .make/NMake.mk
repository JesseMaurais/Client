
!ifdef STD
CFLAGS = $(CFLAGS) /std:$(STD)
!endif

CFLAGS = $(CFLAGS) /permissive- /EHsc

DEP = $**
OUT = /Fe$@
EXE = .exe
OBJ = .obj

RM = del /f
