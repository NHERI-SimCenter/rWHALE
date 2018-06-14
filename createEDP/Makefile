CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include -I ./nanoflann/include
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS=createEDP.cpp HazusLossEstimator.cpp
OBJS=createEDP.o HazusLossEstimator.o

all: createEDP StandardEarthquakeEDP

debug: CPPFLAGS += -DDEBUG -g
debug: all

HazusLossEstimator.o: HazusLossEstimator.cpp HazusLossEstimator.h

StandardEarthquakeEDP.o : StandardEarthquakeEDP.cpp

createEDP.o: createEDP.cpp HazusLossEstimator.h

createEDP: $(OBJS)
	$(CXX) $(LDFLAGS) -o createEDP $(OBJS) $(LDLIBS)

StandardEarthquakeEDP: StandardEarthquakeEDP.o
	$(CXX) $(LDFLAGS) -o StandardEarthquakeEDP StandardEarthquakeEDP.o $(LDLIBS)

clean:
	$(RM) $(OBJS) *~ \#*

distclean: clean
	$(RM) createEDP


