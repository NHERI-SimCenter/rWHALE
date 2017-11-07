CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include -I ./nanoflann/include
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS=createSAM.cpp \
	FloorParam.cpp \
	InterstoryParam.cpp \
	Building.cpp \
	processSAM.cpp \
	HazusSAM_Generator.cpp

OBJS= FloorParam.o InterstoryParam.o Building.o \
	HazusSAM_Generator.o 

all: createSAM processSAM

Building.o: Building.cpp Building.h

HazusSAM_Generator.o: HazusSAM_Generator.cpp

FloorParam.o: FloorParam.cpp FloorParam.h

InterstoryParam.o: InterstoryParam.cpp InterstoryParam.h

createSAM.o: createSAM.cpp

processSAM.o: processSAM.cpp

createSAM: $(OBJS) createSAM.o
	$(CXX) $(LDFLAGS) -o createSAM createSAM.o $(OBJS) $(LDLIBS)

processSAM: $(OBJS) processSAM.o
	$(CXX) $(LDFLAGS) -o processSAM processSAM.o $(OBJS) $(LDLIBS)

clean:
	$(RM) $(OBJS) *~ \#* processSAM.o

distclean: clean
	$(RM) createSAM processSAM


