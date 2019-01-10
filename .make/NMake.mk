
!ifdef STD
CFLAGS = $(CFLAGS) /std:$(STD)
!endif

CFLAGS = /nologo $(CFLAGS) /permissive- /EHsc
COUT = /Fe
EXE = .exe
RM = del /f
