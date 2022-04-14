all: server
	echo Done

clean:
	rm -f server dijkstra.o server.o digraph.o
	echo Cleaned

run: server
	./server < edmonton-roads-2.0.1.txt

server: dijkstra.o server.o digraph.o
	g++ -std=c++11 -g -o server dijkstra.o server.o digraph.o

dijkstra.o: dijkstra.cpp dijkstra.h
	g++ -std=c++11 -g -c dijkstra.cpp

server.o: server.cpp
	g++ -std=c++11 -g -c server.cpp

digraph.o: digraph.cpp digraph.h wdigraph.h
	g++ -std=c++11 -g -c digraph.cpp