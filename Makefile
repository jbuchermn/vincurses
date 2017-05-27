CXX = clang++
SRC=src
OBJECTS = $(patsubst %.cpp,%.o, $(wildcard $(SRC)/*.cpp))

default: libvincurses.a

%.o: %.cpp
	$(CXX) -c $< -o $@

libvincurses.a: $(OBJECTS)
	$(AR) -rcs libvincurses.a $(OBJECTS)

.PHONY: clean
clean:
	rm -f $(SRC)/*.gch
	rm -f $(SRC)/*.o
	rm -f *.a
