CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a

OBJS=csvparser.o 

all: urbanSimDatabase randomUrbanSimDatabase GenericBimDatabase

debug: CPPFLAGS += -DDEBUG -g
debug: all

csvParser.o: csvparser.cpp csvparser.h

randomCreateBIM.o: randomCreateBIM.cpp

countStory.o: countStory.cpp

count: countStory.o
	$(CXX) $(LDFLAGS) -o createBIM countStory.o csvparser.o $(LDLIBS)

urbanSimDatabase: $(OBJS) UrbanSimDatabase.o
	$(CXX) $(LDFLAGS) -o UrbanSimDatabase $(OBJS) UrbanSimDatabase.o $(LDLIBS)

randomUrbanSimDatabase: $(OBJS) RandomUrbanSimDatabase.o
	$(CXX) $(LDFLAGS) -o RandomUrbanSimDatabase RandomUrbanSimDatabase.o $(OBJS) $(LDLIBS)

GenericBimDatabase: $(OBJS) GenericBimDatabase.h GenericBimDatabase.cpp

clean:
	$(RM) $(OBJS) *~ \#* *.o

distclean: clean
	$(RM) UrbanSimDatabase


