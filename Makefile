# Use the C++ preprocessor to generate another Makefile tailored to the
# operating system and version of $(MAKE) used. Forward standard targets
# to it.

MAKEFILE=make/Makefile.mk
TEMPLATE=make/Makefile.cc

all: $(MAKEFILE)
	$(MAKE) -f $(MAKEFILE)

clean: $(MAKEFILE)
	$(MAKE) -f $(MAKEFILE) clean

$(MAKEFILE): $(TEMPLATE)
	$(CXX) $(MAKECONFIG) -E $(TEMPLATE) > $(MAKEFILE)

