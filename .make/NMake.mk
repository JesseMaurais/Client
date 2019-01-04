
!ifdef STD
CFLAGS = $(CFLAGS) /std:$(STD)
!endif

CFLAGS = $(CFLAGS) /permissive- /EHsc
OUT = /Fe
EXE = .exe
OBJ = .obj
RM = del /f
