#
## src/Makefile
#
#CXX=$(shell sst-config --CXX)
CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)

all: libmultiBus.so install

libmultiBus.so: multiBus.cc
        $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

install:
        sst-register multiBus multiBus_LIBDIR=$(CURDIR)
        #sst-register SST_ELEMENT_SOURCE multiBus=$(CURDIR)
        #sst-register SST_ELEMENT_TESTS  multiBus=$(CURDIR)

clean:
        rm -f *.o libmultiBus.so
        rm -Rf *.o $(COMPONENT_LIB)

