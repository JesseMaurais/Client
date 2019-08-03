
CPPCHECKDIR=Cppcheck
CPPCHECKXML=$(CPPCHECKDIR).xml

cppcheck: $(CPPCHECKXML)

$(CPPCHECKXML): $(SRC)
	$(MK) check/Cppcheck
	cppcheck --quiet --errorlist --enable=all $(SRC) > $@
	cppcheck-htmlreport --report-dir=check/Cppcheck --source-dir=. --file=$@

