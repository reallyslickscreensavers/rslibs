INCLUDE = -I..
COMPILE = g++ $(INCLUDE) -std=c++14 -O3
OBJECTS = rsText.o


all: $(OBJECTS)
	ar cr ../lib/rsText.a $(OBJECTS)


.SUFFIXES: .cpp .o

.cpp.o:
	$(COMPILE) -o $@ -c $<
