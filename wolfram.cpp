#include <SFML/Graphics.hpp>
#include <iostream>
#include <bitset>
#include <vector>
#include <thread>
#include <mutex>

#define ANCHO_TABLERO 1000
#define ALTO_VENTANA 700
#define TAM 500

std::mutex mu;

int mod(const int a, const int m){
    return a%m>=0?a%m:a%m+m;
}

template<size_t N>
void avanzar(std::bitset<N> &fila, unsigned char &configuracion, bool &play, int &delay, int &gen){
    while(true){
    if(play){
        mu.lock();
        gen++;
        std::bitset<N> anterior(fila);
        for(int i = 0; i<N; i++){
            fila[i]=(configuracion>>(anterior[mod(i-1, N)]<<2 | anterior[i]<<1 | anterior[mod(i+1, N)]))&1;
        }
        mu.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
    }
}

int main(){
    unsigned char configuracion = 30;
    std::bitset<TAM> fila;
    fila[TAM/2]=1;
    sf::RenderWindow window(sf::VideoMode(ANCHO_TABLERO, ALTO_VENTANA), "AC 1 dimension");
    const int tam_celula = ANCHO_TABLERO/TAM;
    const sf::Vector2f tam_vector(tam_celula, tam_celula);
    int y = 0;
    bool play = true;
    int delay = 100;
    int gen=0;
    std::thread t(avanzar<TAM>, std::ref(fila), std::ref(configuracion), std::ref(play), std::ref(delay), std::ref(gen));
    t.detach();
    while (window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed){
                window.close();
            }
        }
        mu.lock();
        for(int x = 0; x<TAM; x++){
            sf::RectangleShape celula;
            celula.setPosition(x * tam_celula, y * tam_celula);
            celula.setSize(tam_vector);
            celula.setFillColor(fila[x]?sf::Color::Red:sf::Color::White);
            window.draw(celula);
        }
        mu.unlock();
        std::cout<<gen<<'\n';
        y++;
        window.display();
        sf::sleep(sf::milliseconds(100));
    }
}