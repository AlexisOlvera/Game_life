#include <SFML/Graphics.hpp>
#include <iostream>
#include <bitset>
#include <vector>
#include <thread>
#include <mutex>

#define ANCHO_TABLERO 1000
#define ALTO_VENTANA 700
#define TAM 10000

std::mutex mu;

const sf::Color gris(166,166,166), purple(166, 62, 197), teal(57, 174, 169), teal_black(6, 44, 48);


int mod(const int a, const int m){
    return a%m>=0?a%m:a%m+m;
}

template<size_t N>
void avanzar(std::bitset<N> &fila, unsigned char &configuracion, bool &play, int &delay, int &gen, std::vector<std::bitset<N>> &historial){
    while(true){
    if(play){
        mu.lock();
        gen++;
        std::bitset<N> anterior(fila);
        for(int i = 0; i<N; i++){
            fila[i]=(configuracion>>(anterior[mod(i-1, N)]<<2 | anterior[i]<<1 | anterior[mod(i+1, N)]))&1;
        }
        historial.push_back(fila);
        mu.unlock();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}

int main(){
    unsigned char configuracion = 30;
    std::bitset<TAM> fila;
    std::vector<std::bitset<TAM>> historial;
    fila[1]=1;
    sf::RenderWindow window(sf::VideoMode(ANCHO_TABLERO, ALTO_VENTANA), "AC 1 dimension");
    const int tam_celula = 5;//ANCHO_TABLERO/TAM;
    const sf::Vector2f tam_vector(tam_celula, tam_celula);
    const float ancho_total = ANCHO_TABLERO;

    sf::Color colores[] = {sf::Color::Black, sf::Color::Cyan, sf::Color::Green,
        sf::Color::Magenta, sf::Color::White, sf::Color::Red, sf::Color::Yellow, 
        sf::Color::Blue, purple, teal, teal_black};
    int8_t i_c_vivas=0, i_c_muertas=4;
    sf::Color c_vivas=colores[i_c_vivas], c_muertas=colores[i_c_muertas];
    sf::View view_tablero(sf::FloatRect(0.f, 0.f, static_cast<float>(ANCHO_TABLERO), static_cast<float> (ALTO_VENTANA)));
    view_tablero.setViewport(sf::FloatRect(0.f, 0.f, ANCHO_TABLERO/ancho_total, 1.f));
    bool play = true;
    bool siguiente = false;
    int delay = 100;
    int gen=0;
    std::thread t(avanzar<TAM>, std::ref(fila), std::ref(configuracion), std::ref(play), std::ref(delay), std::ref(gen), std::ref(historial));
    t.detach();
    
    while (window.isOpen()){
        if(siguiente){
            siguiente=false;
            play=false;
        }
        sf::Event event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed){
                window.close();
            }
            if(event.type == sf::Event::KeyPressed){
                switch (event.key.code)
                {
                case sf::Keyboard::A:
                    view_tablero.move(sf::Vector2f(-5.f, 0.f));
                    break;
                case sf::Keyboard::W:
                    view_tablero.move(sf::Vector2f(0.f, -5.f));
                    break;
                case sf::Keyboard::S:
                    view_tablero.move(sf::Vector2f(0.f, 5.f));
                    break;
                case sf::Keyboard::D:
                    view_tablero.move(sf::Vector2f(5.f, 0.f));
                    break;
                case sf::Keyboard::Add:
                        view_tablero.zoom(0.8f);
                    break;
                case sf::Keyboard::Subtract:
                        view_tablero.zoom(1.2f);
                    break;
                case sf::Keyboard::P:
                        play=!play;
                    break;
                case sf::Keyboard::N:
                        siguiente=true;
                        play=true;
                    break;
                case sf::Keyboard::C:
                    i_c_vivas=(i_c_vivas+1)%(sizeof(colores)/sizeof(*colores));
                    c_vivas=colores[i_c_vivas];
                    break;
                case sf::Keyboard::X:
                    i_c_muertas=(i_c_muertas+1)%(sizeof(colores)/sizeof(*colores));
                    c_muertas=colores[i_c_muertas];
                    break;
                default:
                    break;
                }
            }
        }
        window.clear();
        window.setView(view_tablero);
        mu.lock();
        sf::RectangleShape muertas;
        muertas.setPosition(0, 0);
        muertas.setSize(sf::Vector2f(ALTO_VENTANA*tam_celula, ANCHO_TABLERO*tam_celula));
        muertas.setFillColor(c_muertas);
        window.draw(muertas);
        for(int y = 0; y<historial.size(); y++)
        for(int x = 0; x<TAM; x++){
            if(historial[y][x]){ //vivas
                sf::RectangleShape celula;
                celula.setPosition(x * tam_celula, y * tam_celula);
                celula.setSize(tam_vector);
                celula.setFillColor(c_vivas);
                window.draw(celula);
            }
        }
        mu.unlock();
        window.display();
        sf::sleep(sf::milliseconds(100));
    }
}