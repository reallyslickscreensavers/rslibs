INCLUDE  = -I..
CXX      ?= g++
CXXFLAGS ?= -std=c++14 -O3 -msse -mfpmath=sse
CPPFLAGS ?= -DGL_GLEXT_PROTOTYPES
OBJECTS  = impCubeTables.o \
	impCubeVolume.o \
	impSurface.o \
	impShape.o \
	impSphere.o \
	impEllipsoid.o \
	impTorus.o \
	impHexahedron.o \
	impRoundedHexahedron.o \
	impCapsule.o \
	impKnot.o


all: $(OBJECTS)
	ar cr ../lib/Implicit.a $(OBJECTS)

clean:
	rm ${OBJECTS}


.SUFFIXES: .cpp .o

.cpp.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<
