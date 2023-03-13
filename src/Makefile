CXXFLAGS=$(shell sst-config --ELEMENT_CXXFLAGS)
LDFLAGS=$(shell sst-config --ELEMENT_LDFLAGS)
ICORE=/mnt/data/sst/core/include
IELEMENTS=/mnt/data/sst/elements/include

all: libmyComponent.so install

libmyComponent.so: myComponent.cc
	$(CXX) $(CXXFLAGS) $(ICORE) $(IELEMENTS) $(LDFLAGS) -o $@ $<

install:
	sst-register myComponent myComponent_LIBDIR=$(CURDIR)

clean:
	rm -f *.o libmyComponent.so
	rm -Rf *.o $(COMPONENT_LIB)
