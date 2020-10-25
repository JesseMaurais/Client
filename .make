#line 1 ".make.cc"

























































#line 59 ".make.cc"


!if "$(OS)"=="Windows_NT"
CFLAGS=$(CFLAGS) -D_WIN32
!ifdef WINVER
CFLAGS=$(CFLAGS) "-D_WIN32_WINNT=$(WINVER)"
!endif
OUTEXT=exe
RM=del /f
MKD=md
DIR=\ 
ENT=;
!else 
CFLAGS=$(CFLAGS) -D_POSIX_SOURCE
!ifdef UNIVER
CFLAGS=$(CFLAGS) "-D_XOPEN_SOURCE=$(UNIVER)"
!endif
LDFLAGS=$(LDFLAGS) -lm -ldl -lrt -lpthread
OUTEXT=out
RM=rm -f
MKD=mkdir -p
DIR=/
ENT=:
!endif


!ifndef STD

STD=c++latest


#line 91 ".make.cc"
!endif
MAKDIR=obj$(DIR)
OBJDIR=obj$(DIR)
SRCDIR=src$(DIR)
HDRDIR=$(SRCDIR)
DEPDIR=$(OBJDIR)
PCH=pre
SRCEXT=cpp
HDREXT=hpp
MAKEXT=mak
ALLSRC=$(SRCDIR)*.$(SRCEXT)
ALLHDR=$(SRCDIR)*.$(HDREXT)

.SUFFIXES: .$(SRCEXT) .$(HDREXT) .$(MAKEXT)



!ifdef COMSPEC
MAKHDR=$(MAKDIR)header.$(MAKEXT)
!if ![(echo HDR=\>$(MAKHDR)) && for %i in ($(ALLHDR)) do @echo %i\>>$(MAKHDR)]
!include $(MAKHDR)
!endif 
MAKSRC=$(MAKDIR)source.$(MAKEXT)
!if ![(echo SRC=\>$(MAKSRC)) && for %i in ($(ALLSRC)) do @echo %i\>>$(MAKSRC)]
!include $(MAKSRC)
!endif 
MAKEXE=$(MAKDIR)target.$(MAKEXT)
!if ![(echo EXE=\>$(MAKEXE)) && for /f %i in ('findstr /s /m "\<main\>" *.$(SRCEXT)') do @echo %~ni.$(OUTEXT)\>>$(MAKEXE)]
!include $(MAKEXE)
!endif 
MAKINC=$(MAKDIR)include.$(MAKEXT)
!if ![(echo INC=\>$(MAKINC)) && for %i in ("%INCLUDE:$(ENT)=" "%") do @echo -I"%~i"\>>$(MAKINC)]
!include $(MAKINC)
!endif 
!endif 





#line 132 ".make.cc"





CFLAGS=$(CFLAGS) $(INC)



OBJEXT=obj
DEPEXT=dep
INLEXT=inl
LIBEXT=lib
PCHEXT=pch


CFLAGS=$(CFLAGS) -nologo -DNOMINMAX -EHsc -permissive-
LDFLAGS=$(LDFLAGS) -nologo
CXXFLAGS=$(CXXFLAGS) $(CFLAGS)


!ifdef STD
CFLAGS=$(CFLAGS) -std:$(STD)
!endif


!ifndef NDEBUG
CFLAGS=$(CFLAGS) -Z7 -W4 -D_CRT_SECURE_NO_WARNINGS
LDFLAGS=$(LDFLAGS) -Z7
!endif


!ifdef HDRDIR
CFLAGS=$(CFLAGS) -I$(HDRDIR)
!endif


!ifdef PCH
PCHHDR=$(SRCDIR)$(PCH).$(HDREXT)
PCHSRC=$(SRCDIR)$(PCH).$(SRCEXT)
PCHOBJ=$(OBJDIR)$(PCH).$(OBJEXT)
PCHOUT=$(SRCDIR)$(PCH).$(PCHEXT)
CFLAGS=$(CFLAGS) -FI$(PCH).$(HDREXT) -Fp$(PCHOUT)
CXXFLAGS=$(CXXFLAGS) -Yu$(PCH).$(HDREXT)
LDFLAGS=$(LDFLAGS) -Yu$(PCH).$(HDREXT)
$(PCHOBJ): $(PCHHDR); $(CXX) $(CFLAGS) -Yc$(PCH).$(HDREXT) -c $(PCHSRC) -Fo$(PCHOBJ)
!endif


CXXCMD=$(CXX) $(CXXFLAGS) -c $< -Fo$(OBJDIR)
LNKCMD=$(CXX) $(LDFLAGS) $(OBJ) -Fe$@
LNKDEP=$(PCHOBJ) $(OBJ) $(DEP)



!ifdef COMSPEC
{$(SRCDIR)}.$(SRCEXT){$(DEPDIR)}.$(DEPEXT):
	@echo $< : \> $@
	@set CMD=$(CXX) $(CFLAGS) -showIncludes -Zs -c $<
	@for /F "tokens=1,2,3,*" %%A in ('%CMD%') do @if not "%%D"=="" @echo "%%D" \>> $@
!endif
{$(SRCDIR)}.$(SRCEXT){$(OBJDIR)}.$(OBJEXT):: ; $(CXXCMD)


#line 197 ".make.cc"






















































#line 252 ".make.cc"

.SUFFIXES: .$(OUTEXT) .$(OBJEXT) .$(DEPEXT) .$(LIBEXT) .$(INLEXT) .$(PCHEXT)



!ifdef COMSPEC
MAKDEP=$(MAKDIR)depend.$(MAKEXT)
!if ![(echo DEP=\>$(MAKDEP)) && for %I in ($(ALLSRC)) do @echo $(OBJDIR)%~nI.$(DEPEXT)\>>$(MAKDEP)]
!include $(MAKDEP)
!endif 
MAKOBJ=$(MAKDIR)object.$(MAKEXT)
!if ![(echo OBJ=\>$(MAKOBJ)) && for %I in ($(ALLSRC)) do @echo $(OBJDIR)%~nI.$(OBJEXT)\>>$(MAKOBJ)]
!include $(MAKOBJ)
!endif 
!endif 



#line 271 ".make.cc"


all: $(EXE)
$(EXE): $(LNKDEP); $(LNKCMD)
$(OBJDIR): ; $(MKD) $(OBJDIR)
clean: ; $(RM) $(EXE) $(OBJ) $(PCHOBJ) $(DEP) 



!ifdef COMSPEC
MAKALLDEP=$(MAKDIR)all.$(MAKEXT)
!if ![(for %i in ($(DEPDIR)*.$(DEPEXT)) do @echo !include %i) > $(MAKALLDEP)]
!include $(MAKALLDEP)
!endif
!endif


#line 289 ".make.cc"

