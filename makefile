app: graph.cpp
	g++ -c button.cpp
	g++ -c graph.cpp
	g++ graph.o button.o -o app -lsfml-graphics -lsfml-window -lsfml-system
run:
	./app