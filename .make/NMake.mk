
CFLAGS = $(CFLAGS) /nologo

!ifdef STD
CFLAGS = $(CFLAGS) /std:$(STD)
!endif

CFLAGS = $(CFLAGS) /permissive- /EHsc
COUT = /Fe

EXEEXT = .exe
OBJEXT = .obj

DEL = del /f
