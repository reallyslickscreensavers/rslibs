INCLUDE = -I..
COMPILE = g++ $(INCLUDE) -std=c++14 -O3
OBJECTS = rsXScreenSaver.o


all: $(OBJECTS)
	ar cr ../lib/rsXScreenSaver.a $(OBJECTS)

clean:
	rm $(OBJECTS)


.SUFFIXES: .cpp .o

.cpp.o:
	$(COMPILE) -o $@ -c $<
