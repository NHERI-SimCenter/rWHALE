CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include -I ./nanoflann/include
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS=HazusLossEstimator.cpp
OBJS=HazusLossEstimator.o

all:StandardEarthquakeEDP

debug: CPPFLAGS += -DDEBUG -g
debug: all

HazusLossEstimator.o: HazusLossEstimator.cpp HazusLossEstimator.h

StandardEarthquakeEDP.o : StandardEarthquakeEDP.cpp

StandardEarthquakeEDP: StandardEarthquakeEDP.o
	$(CXX) $(LDFLAGS) -o StandardEarthquakeEDP StandardEarthquakeEDP.o $(LDLIBS)

clean:
	$(RM) *.o

distclean: clean
	$(RM) StandardEarthquakeEDP


