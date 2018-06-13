
all:
	cd createBIM; make
	cd createEDP; make
	cd createEVENT; make
	cd createSAM; make
	cd performSIMULATION; make
	cd performUQ; make
	cd createLOSS; make
	cd finalProcessing; make

HazusLossEstimator.o: HazusLossEstimator.cpp HazusLossEstimator.h

createEDP.o: createEDP.cpp HazusLossEstimator.h

createEDP: $(OBJS)
	$(CXX) $(LDFLAGS) -o createEDP $(OBJS) $(LDLIBS)

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


