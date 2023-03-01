CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)

all: libmemrouterConverter.so install

libmemrouterConverter.so: memrouterConverter.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

install:
	sst-register memrouterConverter memrouterConverter_LIBDIR=$(CURDIR)

clean:
	rm -f *.o libmemrouterConverter.so
	rm -Rf *.o $(COMPONENT_LIB)
