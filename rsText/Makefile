INCLUDE = -I..
COMPILE = g++ $(INCLUDE) -O3
OBJECTS = rsText.o


all: $(OBJECTS)
	ar cr ../lib/rsText.a $(OBJECTS)


.SUFFIXES: .cpp .o

.cpp.o:
	$(COMPILE) -o $@ -c $<
