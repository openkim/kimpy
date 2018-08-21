CC = g++
CFLAGS = -O2 -g -Wall -fPIC  -I../../../kimpy/neighlist
LDFLAGS = -L./
LDLIBS = -lm -lneigh

NEIGH_LIB_SOURCES =  ../../../kimpy/neighlist/neighbor_list.cpp
NEIGH_LIB_OBJECTS = $(NEIGH_LIB_SOURCES:.cpp=.o)

SOURCES = graphite.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# linking
graphite: $(OBJECTS) libneigh.so
	$(CC) $(LDFLAGS)  -Wl,-rpath=./ -o $@ $^ $(LDLIBS)

# linking
libneigh.so: $(NEIGH_LIB_OBJECTS)
	$(CC) $(LDFLAGS) -shared -o $@ $^

# creating objects
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY: clean
clean:
	rm *.o *.so graphite *.xyz
