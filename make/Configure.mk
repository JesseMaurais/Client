#line 1 "make/Configure.cc"


















#line 20 "make/Configure.cc"


!if "$(OS)"=="Windows_NT"
EXT=exe
RM=del /f
MKD=md
DIR=\ 
ENT=;
!ifndef WINVER
WINVER=0x0A00
!endif
CFLAGS=$(CFLAGS) -D_WIN32 -D_WIN32_WINNT=$(WINVER)
!else 
EXT=out
RM=rm -f
MKD=mkdir -p
DIR=/
ENT=:
!ifndef UNIVER
UNIVER=700
!endif
CFLAGS=$(CFLAGS) -D_POSIX_SOURCE -D_XOPEN_SOURCE=$(UNIVER)
LDFLAGS=$(LDFLAGS) -lm -ldl -lrt -lpthread
!endif


.SUFFIXES: .cpp .hpp .mak .$(EXT)
STD=c++17
MAKDIR=make$(DIR)
OBJDIR=obj$(DIR)
SRCDIR=src$(DIR)
HDRDIR=$(SRCDIR)
ALLHDR=$(SRCDIR)*.hpp
ALLSRC=$(SRCDIR)*.cpp
PRE=pre.hpp

all: test.$(EXT)



!ifdef COMSPEC
MAKHDR=$(MAKDIR)Header.mak
!if ![echo HDR=\>$(MAKHDR) ] 
!if ![for %i in ($(ALLHDR)) do @echo %i\>>$(MAKHDR) ]
!include $(MAKHDR)
!endif
!endif
MAKSRC=$(MAKDIR)Source.mak
!if ![echo SRC=\>$(MAKSRC) ]
!if ![for %i in ($(ALLSRC)) do @echo %i\>>$(MAKSRC) ]
!include $(MAKSRC)
!endif
!endif
!endif



#line 78 "make/Configure.cc"







.SUFFIXES: .obj .dep. .inl .pch .lib .pdb .ilk
CFLAGS=$(CFLAGS) /nologo /DNOMINMAX /EHsc /permissive-
LDFLAGS=$(LDFLAGS) /nologo
CXXFLAGS=$(CXXFLAGS) $(CFLAGS)


!ifdef STD
CFLAGS=$(CFLAGS) /std:$(STD)
!endif


!ifdef HDRDIR
CFLAGS=$(CFLAGS) /I$(HDRDIR)
!endif


!ifndef NDEBUG
CFLAGS=$(CFLAGS) /Z7 /W4 /D_CRT_SECURE_NO_WARNINGS
LDFLAGS=$(LDFLAGS) /Z7
!endif


!ifdef PRE
PCH=$(PRE:.hpp=.pch)
$(PCH): $(SRCDIR)$(PRE); $(CXX) $(CFLAGS) /Yc$(PRE) /Fe$@ /c $(SRCDIR)$(PRE:.hpp=.cpp)
CCFLAGS=$(CCFLAGS) -Yu$(PCH) -FI$(PRE)
LDFLAGS=$(LDFLAGS) -Yu$(PCH)
!endif



!ifdef COMSPEC
MAKDEP=$(MAKDIR)Depend.mak
!if ![echo DEP=\>$(MAKDEP) ] 
!if ![for %I in ($(ALLSRC)) do @echo $(OBJDIR)%~nI.dep\>>$(MAKDEP) ]
!include $(MAKDEP)
!endif
!endif
MAKOBJ=$(MAKDIR)Object.mak
!if ![echo OBJ=\>$(MAKOBJ) ]
!if ![for %I in ($(ALLSRC)) do @echo $(OBJDIR)%~nI.obj\>>$(MAKOBJ) ]
!include $(MAKOBJ)
!endif
!endif
!endif



#line 134 "make/Configure.cc"
ALLDEP=$(OBJDIR)*.dep


test.$(EXT): $(PCH) $(OBJ) $(DEP); $(CXX) $(LDFLAGS) $(OBJ) -Yu$(PCH) -Fe$@

{$(SRCDIR)}.cpp{$(OBJDIR)}.obj:; $(CXX) $(CXXFLAGS) /c $< /Fo$@


#line 143 "make/Configure.cc"
!ifdef COMSPEC
{$(SRCDIR)}.cpp{$(OBJDIR)}.dep:
	@echo $< : \> $@
	@set CMD=$(CXX) $(CFLAGS) /showIncludes /Zs /c $<
	@for /F "tokens=1,2,3,*" %%A in ('%CMD%') do @if not "%%D"=="" @echo "%%D" \>> $@
!endif




























































#line 210 "make/Configure.cc"


clean: ; $(RM) test.$(EXT) $(OBJ) $(DEP) $(PCH)



!ifdef COMSPEC
MAKALLDEP=$(MAKDIR)All.mak
!if ![(for %i in ($(ALLDEP)) do @echo !include %i) > $(MAKALLDEP)]
!include $(MAKALLDEP)
!endif
!endif


#line 225 "make/Configure.cc"

