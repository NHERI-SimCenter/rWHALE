CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include -I ../nanoflann/include
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS=csvparser.c createEVENT.cpp
OBJS=csvparser.o createEVENT.o

all: createEVENT

csvParser.o: csvparser.cpp csvparser.h

createEVENT.o: createEVENT.cpp

createEVENT: $(OBJS)
	$(CXX) $(LDFLAGS) -o createEVENT $(OBJS) $(LDLIBS)

clean:
	$(RM) $(OBJS) *~ \#*

distclean: clean
	$(RM) createEVENT


