main: main.o Tree.o TreeGenerator.o SubsetDeltaGenerator.o
	g++ -O3 -o main main.o Tree.o TreeGenerator.o SubsetDeltaGenerator.o

main.o: main.cpp Tree.cpp TreeGenerator.cpp SubsetDeltaGenerator.cpp
	g++ -O3 -Wall -c main.cpp

Tree.o: Tree.cpp
	g++ -O3 -Wall -c Tree.cpp

TreeGenerator.o: Tree.cpp TreeGenerator.cpp
	g++ -O3 -Wall -c TreeGenerator.cpp

SubsetDeltaGenerator.o: SubsetDeltaGenerator.cpp
	g++ -O3 -Wall -c SubsetDeltaGenerator.cpp

clean:
	rm -f main *.o

