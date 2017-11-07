CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include -I ./nanoflann/include
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS=createEDP.cpp HazusLossEstimator.cpp
OBJS=createEDP.o HazusLossEstimator.o

all: createEDP

HazusLossEstimator.o: HazusLossEstimator.cpp HazusLossEstimator.h

createEDP.o: createEDP.cpp HazusLossEstimator.h

createEDP: $(OBJS)
	$(CXX) $(LDFLAGS) -o createEDP $(OBJS) $(LDLIBS)

clean:
	$(RM) $(OBJS) *~ \#*

distclean: clean
	$(RM) createEDP


