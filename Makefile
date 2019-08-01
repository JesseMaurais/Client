
all: .make/Makefile
	$(MAKE) -f .make/Makefile

.make/Makefile: .make/Makefile.c
	$(CXX) -E $< > $@

