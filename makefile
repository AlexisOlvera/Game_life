app: graph.cpp
#	g++ -c button.cpp
	g++ -c graph.cpp -std=c++11 
	g++ graph.o button.o -o app -lsfml-graphics -lsfml-window -lsfml-system -pthread
run:
	./app