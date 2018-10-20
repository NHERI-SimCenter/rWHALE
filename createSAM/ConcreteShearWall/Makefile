CC=gcc
CXX=g++ -std=c++11 
RM=rm -f
CPPFLAGS= -I /usr/local/include -I ./nanoflann/include -g
LDFLAGS= 
LDLIBS=/usr/local/lib/libjansson.a

SRCS=ConcreteShearWall.cpp \
	processSAM.cpp 

OBJS= ConcreteShearWall.o 

all: processSAM 

processSAM.o: processSAM.cpp

ConcreteShearWall.o: ConcreteShearWall.cpp

processSAM: $(OBJS) processSAM.o
	$(CXX) $(LDFLAGS) -o processSAM processSAM.o $(OBJS) $(LDLIBS)

createOPS: mainPreprocessor.o OpenSeesWallPreprocessor.o
	$(CXX) $(LDFLAGS) -o mainPreprocessor.o OpenSeesWallPreprocessor.o  $(LDLIBS)

clean:
	$(RM) $(OBJS) *~ \#* processSAM.o

distclean: clean
	$(RM) createSAM processSAM


