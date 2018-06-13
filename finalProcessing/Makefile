CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -std=c++11 -I /usr/local/jansson/include -I ../nanoflann/include
LDFLAGS= -std=c++11
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS=readDLs.cpp
OBJS=readDLs.o

all: readDLs

readDLs.o: readDLs.cpp

readDLs: $(OBJS)
	$(CXX) $(LDFLAGS) -o readDLs $(OBJS) $(LDLIBS)

clean:
	$(RM) $(OBJS) *~ \#*

distclean: clean
	$(RM) readDLs


