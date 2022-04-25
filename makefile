app: graph.cpp
#	g++ -c button.cpp
	g++ -c graph.cpp -std=c++11 
	g++ graph.o button.o -o app -lsfml-graphics -lsfml-window -lsfml-system -pthread
run:
	./app
wolfram: wolfram.cpp
	g++ -c wolfram.cpp -std=c++11
	g++ wolfram.o -o wolfram -lsfml-graphics -lsfml-window -lsfml-system -pthread
grafica: grafica.cpp
	g++ -c grafica.cpp -std=c++11
	g++ grafica.o -o grafica -lsfml-graphics -lsfml-window -lsfml-system

