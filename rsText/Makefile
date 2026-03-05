INCLUDE  = -I..
CXX      ?= g++
CXXFLAGS += -std=c++14 -O3
OBJECTS  = rsText.o


all: $(OBJECTS)
	ar cr ../lib/rsText.a $(OBJECTS)


.SUFFIXES: .cpp .o

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<
