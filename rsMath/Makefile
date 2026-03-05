INCLUDE  = -I..
CXX      ?= g++
CXXFLAGS += -std=c++14 -O3
OBJECTS  = rsVec.o \
	rsVec4.o \
	rsQuat.o \
	rsMatrix.o


all: $(OBJECTS)
	ar cr ../lib/rsMath.a $(OBJECTS)

clean:
	rm ${OBJECTS}

.SUFFIXES: .cpp .o

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<
