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

csvparser.o: csvparser.h

randomCreateBIM.o: randomCreateBIM.cpp

urbanSimDatabase: $(OBJS) UrbanSimDatabase.o
	$(CXX) $(LDFLAGS) -o UrbanSimDatabase $(OBJS) UrbanSimDatabase.o $(LDLIBS)

randomUrbanSimDatabase: $(OBJS) RandomUrbanSimDatabase.o
	$(CXX) $(LDFLAGS) -o RandomUrbanSimDatabase RandomUrbanSimDatabase.o $(OBJS) $(LDLIBS)

GenericBimDatabase: $(OBJS) GenericBimDatabase.o
	$(CXX) $(LDFLAGS) -o GenericBimDatabase GenericBimDatabase.o $(OBJS) $(LDLIBS)

clean:
	$(RM) *.o

distclean: clean
	$(RM) UrbanSimDatabase RandomUrbanSimDatabase GenericBimDatabase