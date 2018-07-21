CC=gcc
CXX=g++
RM=rm -f
LDFLAGS= 
CPPFLAGS= -I /usr/local/jansson/include -I ../include/nanoflann
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS=csvparser.c
OBJS=csvparser.o LLNL_SW4.o

all: LLNL_SW4

debug: CPPFLAGS += -DDEBUG -g
debug: all

csvparser.o: csvparser.h

LLNL_SW4: $(OBJS)
	$(CXX) $(LDFLAGS) -o LLNL_SW4 $(OBJS) $(LDLIBS)

clean:
	$(RM) *.o

distclean: clean
	$(RM) LLNL_SW4


