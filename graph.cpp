#include <SFML/Graphics.hpp>
#include <vector>
#include <bitset>
#include <random>
#define ANCHO_VENTANA 400
#define ALTO_VENTANA 400
#define ANCHO 20
#define ALTO 20

template<size_t N>
void llenarTablero(std::vector<std::bitset<N>> &tablero, double porcentaje){
    int celulas_vivas_inicio = N*N*porcentaje;
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> X(0, N-1);
    std::uniform_int_distribution<> Y(0, N-1);
    while(celulas_vivas_inicio--){
        int x = X(gen), y = Y(gen);
        if(tablero[x][y])
            celulas_vivas_inicio++;    
        tablero[X(gen)].set(Y(gen));
    }
}
template<size_t N>
int cuenta(std::vector<std::bitset<N>> &tablero, int I, int J){
    if(I==0 || J==0 || I==N-1 | J==N-1)
        return 0;
    int suma = 0;
    for(int i=-1; i<2; i++){
        for(int j=-1; j<2; j++){
            if(!(i==0 && j==0))
                suma+=tablero[I+i][J+j]?1:0;
        }
    }
    return suma;
}

template<size_t N>
void avanzar(std::vector<std::bitset<N>> &tablero){
    for(int i = 1; i<N-1; i++){
        for(int j = 1; j<N-1; j++){
            if(tablero[i][j]){
                int suma = cuenta(tablero, i, j);
                if(suma!=2 && suma!=3)
                    tablero[i][j]=false;
            } else{
                if(cuenta(tablero, i, j)==3)
                    tablero[i][j]=true;
            }
        }
    }
}

template<size_t N>
long long int celulas_vivas(std::vector<std::bitset<N>> &tablero){
    long long int cnt = 0;
    for(auto bits : tablero){
        cnt+=bits.count();
    }
    return cnt;
}


int main(int argc, char const *argv[]){
    sf::RenderWindow window(sf::VideoMode(ANCHO_VENTANA, ALTO_VENTANA), "Hello From SFML");
    const int tam_celula = ANCHO_VENTANA/ANCHO;
    const int delay=500;
    const sf::Vector2f tam_vector(tam_celula, tam_celula);
    std::vector<std::bitset<ANCHO>> tablero(ALTO);
    const double porcentaje = 0.1;
    llenarTablero(tablero, porcentaje);
    while (window.isOpen())
    {   
        window.clear();
        sf::Event event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed){
                window.close();
            }
        }
        for(int x = 0; x<ANCHO; x++){
            for(int y = 0; y<ALTO; y++){
                sf::RectangleShape celula;
                celula.setPosition(x * tam_celula, y * tam_celula);
                celula.setSize(tam_vector);
                celula.setOutlineThickness(1);
                celula.setOutlineColor(sf::Color::Blue);
                celula.setFillColor(tablero[x][y]?sf::Color::Black:sf::Color::White);
                window.draw(celula);
            }
        }
        window.display();
        avanzar(tablero);
        sf::sleep(sf::milliseconds(delay));

    }

    return 0;
}