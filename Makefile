# Use the C++ preprocessor to generate another Makefile tailored to the
# operating system and version of $(MAKE) used. Forward standard targets
# to it.

MAKEFILE=.make
TEMPLATE=.make.cc

all: $(MAKEFILE)
	$(MAKE) -f $(MAKEFILE)
clean: $(MAKEFILE)
	$(MAKE) -f $(MAKEFILE) clean
$(MAKEFILE): $(TEMPLATE)
	$(CXX) $(MAKECONFIG) -E $(TEMPLATE) > $(MAKEFILE)

analysis:
	scan-build --use-c++=$(CXX) $(MAKE)

check:
	cppcheck src

pvs-studio:
	pvs-studio-analyzer trace -- $(MAKE)
	pvs-studio-analyzer analyze -o make/PVS.log
	plog-converter make/PVS.log -o make/PVS.html -t html -a 'GA:1,2;64:1;OP:1,2,3;CS:1;MISRA:1,2'

