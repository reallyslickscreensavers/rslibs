INCLUDE = -I..
COMPILE = g++ $(INCLUDE) -std=c++14 -O3
OBJECTS = Rgbhsl.o


all: $(OBJECTS)
	ar cr ../lib/Rgbhsl.a $(OBJECTS)


.SUFFIXES: .cpp .o

.cpp.o:
	$(COMPILE) -o $@ -c $<
