CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include -I ./nanoflann/include
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS=FloorParam.cpp \
	InterstoryParam.cpp \
	Building.cpp \
	HazusSAM_Generator.cpp

OBJS= FloorParam.o InterstoryParam.o Building.o \
	HazusSAM_Generator.o 

all: MDOF

debug: CPPFLAGS += -DDEBUG -g
debug: all

Building.o: Building.cpp Building.h

HazusSAM_Generator.o: HazusSAM_Generator.cpp

FloorParam.o: FloorParam.cpp FloorParam.h

InterstoryParam.o: InterstoryParam.cpp InterstoryParam.h

MDOF: $(OBJS) MDOF_ShearBuildingModel.o
	$(CXX) $(LDFLAGS) $(CPPFLAGS) -o MDOF-LU MDOF_ShearBuildingModel.o $(OBJS) $(LDLIBS)

clean:
	$(RM) *.o

distclean: clean
	$(RM) MDOF-LU


