CXX = g++
CXXFLAGS = -pthread -Wall

release: CXXFLAGS += -O3
release: main

debug: CXXFLAGS += -g -p
debug: main

main: main.o Tree.o TreeGenerator.o DisjointSet.o SubsetDeltaGenerator.o
	$(CXX) $(CXXFLAGS) -o main main.o Tree.o DisjointSet.o TreeGenerator.o SubsetDeltaGenerator.o

main.o: main.cpp Tree.cpp TreeGenerator.cpp DisjointSet.cpp SubsetDeltaGenerator.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

Tree.o: Tree.cpp DisjointSet.cpp
	$(CXX) $(CXXFLAGS) -c Tree.cpp

TreeGenerator.o: Tree.cpp TreeGenerator.cpp
	$(CXX) $(CXXFLAGS) -c TreeGenerator.cpp

DisjointSet.o: DisjointSet.cpp
	$(CXX) $(CXXFLAGS) -c DisjointSet.cpp

SubsetDeltaGenerator.o: SubsetDeltaGenerator.cpp
	$(CXX) $(CXXFLAGS) -c SubsetDeltaGenerator.cpp

clean:
	rm -f main *.o gmon.out

