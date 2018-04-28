CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include -I ./nanoflann/include -std=c++11
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS=	 main.cpp \
	HazusLossEstimator.cpp \
	Building.cpp \
	Component.cpp \
	FragilityCurve.cpp \
	Stat.cpp \
	NormativeQty.cpp \
	NormativeQtyStr.cpp

OBJS= \
	HazusLossEstimator.o \
	Building.o \
	Component.o \
	FragilityCurve.o \
	Stat.o \
	NormativeQty.o \
	NormativeQtyStr.o

otherOBJS = ./include/inifile.o ./include/tinyxml2.o

all: createLOSS FEMA-P58

createLOSS: $(OBJS) main.o
	cd include; make
	$(CXX) $(LDFLAGS) -o createLOSS main.o $(OBJS) $(otherOBJS) $(LDLIBS)

FEMA-P58: $(OBJS) FEMAP58_LU.o
		cd include; make
		$(CXX) $(LDFLAGS) -o FEMAP58_LU FEMAP58_LU.o $(OBJS) $(otherOBJS) $(LDLIBS)


clean:
	cd include; make clean
	$(RM) $(OBJS) *~ \#*

distclean: clean
	cd include; make distclean
	$(RM) createLOSS