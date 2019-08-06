
PVSLOG=$(SRC:.cpp=.log)
PVSINL=$(SRC:.cpp=.i)
PVSHTML=PVS.html

$(PVSHTML): $(SRC) $(PVSLOG)
	plog-converter -a 'GA:1,2;64:1;OP:1,2,3;CS:1;MISRA:1,2' -t html $(PVSLOG) -o $@

pvscheck: $(PVSHTML)

.cpp.log:
	$(CXX) $< -E > $*.i
	pvs-studio --cfg .make/PVS.cfg --i-file $*.i --output-file $@ --source-file $<

