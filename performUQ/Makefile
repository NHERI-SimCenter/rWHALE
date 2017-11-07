CC=gcc
CXX=g++
RM=rm -fr
CPPFLAGS= -I /usr/local/jansson/include -I ./nanoflann/include
LDFLAGS= 
LDLIBS=/usr/local/jansson/lib/libjansson.a

SRCS=extractEDP.cpp postprocessDakota.cpp
OBJS=extractEDP.o  postprocessDakota.o

all: extractEDP postprocessDAKOTA

extractEDP.o: extractEDP.cpp 

postprocessDakota.o: postprocessDakota.cpp 

extractEDP: $(OBJS)
	$(CXX) $(LDFLAGS) -o extractEDP extractEDP.o $(LDLIBS)

postprocessDAKOTA: $(OBJS)
	$(CXX) $(LDFLAGS) -o postprocessDAKOTA postprocessDakota.o $(LDLIBS)

clean:
	$(RM) $(OBJS) *~ \#* extractEDP prostprocessDAKOTA

distclean: clean
	$(RM) extractEDP workdir.* templatedir LHS*.out *.json dakota.*


