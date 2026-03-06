INCLUDE  = -I..
CXX      ?= g++
CXXFLAGS ?= -std=c++14 -O3
OBJECTS  = rsXScreenSaver.o


all: $(OBJECTS)
	ar cr ../lib/rsXScreenSaver.a $(OBJECTS)

clean:
	rm $(OBJECTS)


.SUFFIXES: .cpp .o

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<
