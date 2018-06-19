
all:
	cd createBIM; make
	cd createEDP; make
	cd createEVENT; make
	cd createSAM; make
	cd performSIMULATION; make
	cd performUQ; make
	cd createLOSS; make
	cd finalProcessing; make

debug:
	cd createBIM; make debug
	cd createEDP; make debug
	cd createEVENT; make debug
	cd createSAM; make debug
	cd performSIMULATION; make debug
	cd performUQ; make debug
	cd createLOSS; make debug
	cd finalProcessing; make debug

clean:
	cd createBIM; make clean
	cd createEVENT; make clean
	cd createSAM; make clean
	cd createEDP; make clean
	cd performSIMULATION; make clean
	cd createLOSS; make clean
	cd finalProcessing; make clean
	$(RM) $(OBJS) *~ \#*

distclean: clean
	cd createBIM; make distclean
	cd createEVENT; make distclean
	cd createSAM; make distclean
	cd createEDP; make distclean
	cd performSIMULATION; make distclean
	cd createLOSS; make distclean
	cd finalProcessing; make distclean


