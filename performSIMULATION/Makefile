CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS= -I /usr/local/jansson/include -I ./nanoflann/include
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS=mainPostprocessor.o mainPreprocessor.cpp \
	OpenSeesPreprocessor.cpp OpenSeesPostprocessor.cpp

OBJS_PRE=mainPreprocessor.o OpenSeesPreprocessor.o
OBJS_POST=mainPostprocessor.o OpenSeesPostprocessor.o
OBJS_Uncertainty=getUncertainty.o OpenSeesPreprocessor.o

all: mainPreprocessor mainPostprocessor getUncertainty

OpenSeesPreprocessor.o: OpenSeesPreprocessor.cpp OpenSeesPreprocessor.h

OpenSeesPostprocessor.o: OpenSeesPostprocessor.cpp OpenSeesPostprocessor.h

mainPreprocessor.o: mainPreprocessor.cpp OpenSeesPreprocessor.h

mainPostprocessor.o: mainPostprocessor.cpp OpenSeesPostprocessor.h

OpenSeesPreprocessor.o: getUncertainty.cpp

getUncertainty: $(OBJS_Uncertainty)
	$(CXX) $(LDFLAGS) -o getUncertainty $(OBJS_Uncertainty) $(LDLIBS)

mainPreprocessor: $(OBJS_PRE)
	$(CXX) $(LDFLAGS) -o mainPreprocessor $(OBJS_PRE) $(LDLIBS)

mainPostprocessor: $(OBJS_POST)
	$(CXX) $(LDFLAGS) -o mainPostprocessor $(OBJS_POST) $(LDLIBS)

clean:
	$(RM) *~ \#* *.o *.out

distclean: clean
	$(RM) mainPreprocessor mainPostprocessor getUncertainty


