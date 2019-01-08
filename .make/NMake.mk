
!ifdef STD
CFLAGS = $(CFLAGS) /std:$(STD)
!endif

CFLAGS = $(CFLAGS) /permissive- /EHsc

EXEEXT = .exe
OBJEXT = .obj

RM = del /f
