
!ifdef STD
CFLAGS = $(CFLAGS) /std:$(STD)
!endif

CFLAGS = $(CFLAGS) /permissive- /EHsc /analyze:log .make/CL-nativecodeanalysis.xml

DEP = $**
OUT = /Fe$@
EXE = .exe
OBJ = .obj

RM = del /f
