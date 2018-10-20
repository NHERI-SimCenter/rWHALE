CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/include -I ./nanoflann/include 
LDFLAGS= 
LDLIBS=/usr/local/lib/libjansson.a

SRCS=mainPostprocessor.o mainPreprocessor.cpp \
	OpenSeesPreprocessor.cpp 

OBJS_PRE=mainPreprocessor.o OpenSeesPreprocessor.o

all: mainPreprocessor

debug: CPPFLAGS += -DDEBUG -g
debug: all

OpenSeesPreprocessor.o: OpenSeesPreprocessor.cpp OpenSeesPreprocessor.h


mainPreprocessor.o: mainPreprocessor.cpp OpenSeesPreprocessor.h


mainPreprocessor: $(OBJS_PRE)
	$(CXX) $(LDFLAGS) -o mainPreprocessor $(OBJS_PRE) $(LDLIBS) 

clean:
	$(RM) *~ \#* *.o *.out

distclean: clean
	$(RM) mainPreprocessor 


