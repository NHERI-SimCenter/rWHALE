CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a

OBJS=csvparser.o UrbanSimDatabase.o

all: urbanSimDatabase

csvParser.o: csvparser.cpp csvparser.h

randomCreateBIM.o: randomCreateBIM.cpp

countStory.o: countStory.cpp

count: countStory.o
	$(CXX) $(LDFLAGS) -o createBIM countStory.o csvparser.o $(LDLIBS)

urbanSimDatabase: $(OBJS)
	$(CXX) $(LDFLAGS) -o UrbanSimDatabase $(OBJS) $(LDLIBS)

clean:
	$(RM) $(OBJS) *~ \#* *.o

distclean: clean
	$(RM) UrbanSimDatabase


