CC=gcc
CXX=g++
RM=rm -f
LDFLAGS= 
CPPFLAGS= -I /usr/local/jansson/include -I ../include/nanoflann
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS=csvparser.c createEVENT.cpp
OBJS=csvparser.o createEVENT.o
OBJS1=csvparser.o LLNL_SW4.o

all: createEVENT LLNL_SW4

debug: CPPFLAGS += -DDEBUG -g
debug: all

csvParser.o: csvparser.cpp csvparser.h

createEVENT.o: createEVENT.cpp

createEVENT: $(OBJS)
	$(CXX) $(LDFLAGS) -o createEVENT $(OBJS) $(LDLIBS)

LLNL_SW4: $(OBJS1)
	$(CXX) $(LDFLAGS) -o LLNL_SW4 $(OBJS1) $(LDLIBS)

clean:
	$(RM) $(OBJS) *~ \#*

distclean: clean
	$(RM) createEVENT


