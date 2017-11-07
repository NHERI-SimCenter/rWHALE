CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include -I ./nanoflann/include -std=c++11
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS= inifile.cpp tinyxml2.cpp

OBJS= inifile.o tinyxml2.o

all: $(OBJS)

clean:
	$(RM) $(OBJS) *~ \#*

distclean: clean
	$(RM) createEDP


