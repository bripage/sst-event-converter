CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)

all: libeventConverter.so install

libeventConverter.so: eventConverter.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

install:
	sst-register eventConverter eventConverter_LIBDIR=$(CURDIR)

clean:
	rm -f *.o libeventConverter.so
	rm -Rf *.o $(COMPONENT_LIB)
