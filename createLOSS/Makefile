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

OBJS= main.o \
	HazusLossEstimator.o \
	Building.o \
	Component.o \
	FragilityCurve.o \
	Stat.o \
	NormativeQty.o \
	NormativeQtyStr.o

otherOBJS = ./include/inifile.o ./include/tinyxml2.o

all: createLOSS

HazusLossEstimator.o: HazusLossEstimator.cpp HazusLossEstimator.h


createLOSS: $(OBJS)
	cd include; make
	$(CXX) $(LDFLAGS) -o createLOSS $(OBJS) $(otherOBJS) $(LDLIBS)

clean:
	cd include; make clean
	$(RM) $(OBJS) *~ \#*

distclean: clean
	cd include; make distclean
	$(RM) createLOSS