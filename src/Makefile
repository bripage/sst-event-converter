CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)

all: libmyComponent.so install

libmyComponent.so: myComponent.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

install:
	sst-register myComponent myComponent_LIBDIR=$(CURDIR)

clean:
	rm -f *.o libmyComponent.so
	rm -Rf *.o $(COMPONENT_LIB)
