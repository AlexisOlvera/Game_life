#include <SFML/Graphics.hpp>
#include "button.hpp"
#include <vector>
#include <bitset>
#include <random>
#include <iostream>
#define ANCHO_VENTANA 600
#define ALTO_VENTANA 600
#define ANCHO 30
#define ALTO 30

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
void limpiarTablero(std::vector<std::bitset<N>> &tablero){
    for(auto &fila : tablero){
        fila.reset();
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
    std::vector<std::bitset<N>> anterior(N);
    copy(tablero.begin(), tablero.end(), anterior.begin());
    for(int i = 1; i<N-1; i++){
        for(int j = 1; j<N-1; j++){
            if(anterior[i][j]){
                int suma = cuenta(anterior, i, j);
                if(suma!=2 && suma!=3)
                    tablero[i][j]=false;
            } else{
                if(cuenta(anterior, i, j)==3)
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
    const int tam_celula = ANCHO_VENTANA/ANCHO;
    int delay=1000;
    const sf::Vector2f tam_vector(tam_celula, tam_celula);
    std::vector<std::bitset<ANCHO+2>> tablero(ALTO+2);
    const double porcentaje = 0.1;
    float zoom = 1.0f;
    llenarTablero(tablero, porcentaje);
    sf::RenderWindow window(sf::VideoMode(ANCHO_VENTANA+ANCHO_BOTONES, ALTO_VENTANA), "Juego de la vida");
    sf::Font myfont;
    sf::View view_tablero(sf::FloatRect(0.f, 0.f, 950.f, 600.f));
    sf::View view_botones(sf::FloatRect(0.f,0.f, 950.f, 600.f));
    view_tablero.zoom(zoom);
    if(!myfont.loadFromFile("MesloLGS.ttf")){
        std::cerr<<"Could not find contb.ttf font."<<std::endl;
    }
    
    gui::button parar("Parar", myfont, sf::Vector2f(ANCHO_VENTANA+ANCHO_BOTONES/2, 30.f), gui::style::clean);
    gui::button continuar("Continuar", myfont, sf::Vector2f(ANCHO_VENTANA+ANCHO_BOTONES/2, 80.f), gui::style::clean);
    gui::button mas_velocidad("+ Velocidad", myfont, sf::Vector2f(ANCHO_VENTANA+ANCHO_BOTONES/2, 130.f), gui::style::clean);
    gui::button menos_velocidad("- Velocidad", myfont, sf::Vector2f(ANCHO_VENTANA+ANCHO_BOTONES/2, 180.f), gui::style::clean);
    gui::button mas_zoom("+ Zoom", myfont, sf::Vector2f(ANCHO_VENTANA+ANCHO_BOTONES/2, 230.f), gui::style::clean);
    gui::button menos_zoom("- Zoom", myfont, sf::Vector2f(ANCHO_VENTANA+ANCHO_BOTONES/2, 280.f), gui::style::clean);
    gui::button limpiar("Limpiar", myfont, sf::Vector2f(ANCHO_VENTANA+ANCHO_BOTONES/2, 330.f), gui::style::clean);
    gui::button rellenar("Rellenar", myfont, sf::Vector2f(ANCHO_VENTANA+ANCHO_BOTONES/2, 380.f), gui::style::clean);
    gui::button btn_siguiente("Siguiente", myfont, sf::Vector2f(ANCHO_VENTANA+ANCHO_BOTONES/2, 430.f), gui::style::clean);
    gui::button guardar("Guardar", myfont, sf::Vector2f(ANCHO_VENTANA+ANCHO_BOTONES/2, 480.f), gui::style::clean);
    bool play=true;
    bool siguiente = false;
    while (window.isOpen())
    {   
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
            if(event.type == sf::Event::MouseButtonPressed){
                if(event.mouseButton.x > ANCHO_VENTANA){
                    if(event.mouseButton.y < 70.f) //parar
                        play=false;
                    else if(event.mouseButton.y < 120.f ) //continuar
                        play=true;
                    else if(event.mouseButton.y < 170.f ) //mas velocidad
                        delay-=delay>0?100:0;
                    else if(event.mouseButton.y < 220.f ) //menos velocidad
                        delay+=100;
                    else if(event.mouseButton.y < 270.f ){ // zoom in
                        view_tablero.zoom(0.8);
                        view_tablero.move(sf::Vector2f(25.f, 0.f));
                    }
                    else if(event.mouseButton.y < 320.f ) { // zoom out
                        view_tablero.zoom(1.2);
                        view_tablero.move(sf::Vector2f(-25.f, 0.f));
                    }
                    else if(event.mouseButton.y < 370.f ) // limpiar
                        limpiarTablero(tablero);
                    else if(event.mouseButton.y < 420.f ) // rellenar
                        llenarTablero(tablero, porcentaje);
                    else if(event.mouseButton.y < 470.f ){ //Siguiente
                        siguiente=true;
                        play=true;
                    }
                    else if(event.mouseButton.y < 520.f ){ //guardar
                        std::cout<<"Guardar\n";
                    }
                } else{
                    int X = event.mouseButton.x/tam_celula;
                    int Y = event.mouseButton.y/tam_celula;
                    tablero[X][Y]=!tablero[X][Y];
                }
            }
        }
        window.clear();
        window.setView(view_botones);
        parar.update(event, window);
        continuar.update(event, window);
        mas_velocidad.update(event, window);
        menos_velocidad.update(event, window);
        mas_zoom.update(event, window);
        menos_zoom.update(event, window);
        limpiar.update(event, window);
        rellenar.update(event, window);
        btn_siguiente.update(event, window);
        guardar.update(event, window);
        
        window.draw(parar);
        window.draw(continuar);
        window.draw(mas_velocidad);
        window.draw(menos_velocidad);
        window.draw(mas_zoom);
        window.draw(menos_zoom);
        window.draw(limpiar);
        window.draw(rellenar);
        window.draw(btn_siguiente);
        window.draw(guardar);

        window.setView(view_tablero);
        for(int x = 0; x<ANCHO+1; x++){
            for(int y=0; y<ALTO+1; y++){
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
        if(play)
            avanzar(tablero);
        sf::sleep(sf::milliseconds(delay));

    }

    return 0;
}