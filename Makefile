# Use the C preprocessor to generate another Makefile tailored to the
# operating system and version of $(MAKE) used. Forward standard targets
# to it.

all: .make/Makefile
	$(MAKE) -s -f .make/Makefile

clean: .make/Makefile
	$(MAKE) -s -f .make/Makefile clean

.make/Makefile: .make/Makefile.cc
	$(CXX) -E $< > $@

