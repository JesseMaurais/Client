# Uses the C++ preprocessor to generate another Makefile tailored to the
# operating system, compiler and version of $(MAKE) used.

MAKEFILE=.make
TEMPLATE=.make.cc

# \
MAKECONFIG=-nologo -D_NMAKE
# \
MAKEFLAGS=-nologo

all: $(MAKEFILE)
	$(MAKE) $(MAKEFLAGS) -f $(MAKEFILE) all

help: $(MAKEFILE)
	$(MAKE) $(MAKEFLAGS) -f $(MAKEFILE) help

info: $(MAKEFILE)
	$(MAKE) $(MAKEFLAGS) -f $(MAKEFILE) info

clean: $(MAKEFILE)
	$(MAKE) $(MAKEFLAGS) -f $(MAKEFILE) clean

cflags: $(MAKEFILE)
	$(MAKE) $(MAKEFLAGS) -f $(MAKEFILE) cflags

ctags: $(MAKEFILE)
	$(MAKE) $(MAKEFLAGS) -f $(MAKEFILE) ctags

config:
	$(CXX) $(MAKECONFIG) -E $(TEMPLATE) > $(MAKEFILE)

$(MAKEFILE): $(TEMPLATE)
	$(CXX) $(MAKECONFIG) -E $(TEMPLATE) > $(MAKEFILE)

scan:
	scan-build --use-c++=$(CXX) $(MAKE)

check:
	cppcheck src

verify:
	pvs-studio-analyzer trace -- $(MAKE)
	pvs-studio-analyzer analyze -o PVS.log
	plog-converter PVS.log -o PVS.html -t html -a 'GA:1,2;64:1;OP:1,2,3;CS:1;MISRA:1,2'
