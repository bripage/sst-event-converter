#
## src/Makefile
#
#CXX=$(shell sst-config --CXX)
CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)

all: libbasicComponent.so install

libbasicComponent.so: basicComponent.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

install:
	sst-register basicComponent basicComponent_LIBDIR=$(CURDIR)

clean:
	rm -f *.o libbasicComponent.so
	rm -Rf *.o $(COMPONENT_LIB)

