CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS=csvparser.c createBIM.cpp
OBJS=csvparser.o createBIM.o

all: createBIM

csvParser.o: csvparser.cpp csvparser.h

randomCreateBIM.o: randomCreateBIM.cpp

countStory.o: countStory.cpp

count: countStory.o
	$(CXX) $(LDFLAGS) -o createBIM countStory.o csvparser.o $(LDLIBS)

createBIM: $(OBJS)
	$(CXX) $(LDFLAGS) -o createBIM $(OBJS) $(LDLIBS)

clean:
	$(RM) $(OBJS) *~ \#* *.o

distclean: clean
	$(RM) createBIM


