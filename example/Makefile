CXX = clang++ -lncurses -lvincurses -L../ -I../include

default: main

%.o: %.cpp 
	$(CXX) -c $< -o $@

main: main.o
	cd ../ && $(MAKE)
	$(CXX) main.o -o main 

.PHONY: clean
clean:
	rm -f *.o
	rm -f main
