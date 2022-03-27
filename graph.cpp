#include <SFML/Graphics.hpp>
#include "button.hpp"
#include <vector>
#include <bitset>
#include <random>
#include <iostream>
#include <fstream>
#include <math.h>
#include <thread>
#include <mutex>
#define ANCHO_TABLERO 600
#define ANCHO_GRAFICAS 300
#define ALTO_VENTANA 600
#define ANCHO 50
#define ALTO 50

std::mutex mu;

std::string date(){
    // current date/time based on current system
   time_t now = time(0);
   
   // convert now to string form
   char* dt = ctime(&now);

   // convert now to tm struct for UTC
   tm *gmtm = gmtime(&now);
   dt = asctime(gmtm);
   return dt;
} 

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

int mod(int a, const int m){
    a%=m;
    return a<0?a+m:a;
}

template<size_t N>
double Shannon_entropy(const std::vector<std::bitset<N>> &tablero){
    int configuracion=0;
    std::vector<int> Q_i(512,0);
    for(size_t i = 0; i<N; i++){
        for(size_t j=0; j<N; j++){
            configuracion=0;
            for(int a=-1; a<=1; a++){
                for(int b=-1; b<=1; b++){
                    configuracion|=tablero[mod(i+a, N)][mod(j+b, N)]<<((a+1)*3+(b+1));
                }
            }
            Q_i[configuracion]++;
        }        
    }
    double res = 0;
    for(auto q : Q_i){
        if(q){
            const double q_n = q/((double) N*N);
            res+=(q_n)*log2(q_n);
        }
    }
    return -res;
    
}

template<size_t N>
void avanzar(std::vector<std::bitset<N>>& tablero, bool& play, int& delay){
    while(true){
    if(play){
        mu.lock();
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
        mu.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
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

template<size_t N>
void guardar_tablero(std::vector<std::bitset<N>> &tablero){
    std::ofstream salida;
    salida.open ("salida"+date()+".game_life");
    for(auto &fila:tablero){
        salida<<fila<<"\n";
    }
    salida.close();
}

/*template<size_t N>
std::vector<std::bitset<N>> abrir_tablero(std::string nombre){
    std::ifstream entrada;
    entrada.open("nombre");
    std::string linea;
    getline(entrada, linea);
    int n = linea.size();
    std::vector<std::bitset<n>> tablero;
    do{
        std::bitset<n> fila(linea);
        tablero.emplace_back(fila);
    }while(getline(entrada, linea));
}
*/


const sf::Color gris(166,166,166), purple(166, 62, 197), teal(57, 174, 169), teal_black(6, 44, 48);


int main(int argc, char const *argv[]){
    const int tam_celula = ANCHO_TABLERO/ANCHO;
    int delay=1000;
    const sf::Vector2f tam_vector(tam_celula, tam_celula);
    std::vector<std::bitset<ANCHO+2>> tablero(ALTO+2);
    const double porcentaje = 0.1;
    llenarTablero(tablero, porcentaje);
    sf::RenderWindow window(sf::VideoMode(ANCHO_TABLERO+ANCHO_BOTONES+ANCHO_GRAFICAS, ALTO_VENTANA), "Juego de la vida");
    sf::Font myfont;
    sf::Color c_vivas=sf::Color::Black, c_muertas=sf::Color::White;
    sf::Color colores[] = {sf::Color::Black, sf::Color::Cyan, sf::Color::Green,
        sf::Color::Magenta, sf::Color::White, sf::Color::Red, sf::Color::Yellow, 
        sf::Color::Blue, purple, teal, teal_black};
    int8_t i_c_vivas=0, i_c_muertas=4;

    sf::View view_tablero(sf::FloatRect(0.f, 0.f, static_cast<float>(ANCHO_TABLERO), static_cast<float> (ALTO_VENTANA)));
    sf::View view_botones(sf::FloatRect(static_cast<float>(ANCHO_TABLERO), 0.f, static_cast<float>(ANCHO_BOTONES), static_cast<float> (ALTO_VENTANA)));
    sf::View view_graficas(sf::FloatRect(static_cast<float>(ANCHO_TABLERO+ANCHO_BOTONES), 0.f, static_cast<float>(ANCHO_GRAFICAS), static_cast<float> (ALTO_VENTANA)));
    const float ancho_total = ANCHO_TABLERO+ANCHO_BOTONES+ANCHO_GRAFICAS; 
    view_tablero.setViewport(sf::FloatRect(0.f, 0.f, ANCHO_TABLERO/ancho_total, 1.f));
    view_botones.setViewport(sf::FloatRect(ANCHO_TABLERO/ancho_total, 0.f, ANCHO_BOTONES/ancho_total, 1.f));
    view_graficas.setViewport(sf::FloatRect((ANCHO_TABLERO+ANCHO_BOTONES)/ancho_total, 0.f, ANCHO_GRAFICAS/ancho_total, 1.f));
    if(!myfont.loadFromFile("MesloLGS.ttf")){
        std::cerr<<"Could not find contb.ttf font."<<std::endl;
    }

    
    gui::button parar("Parar", myfont, sf::Vector2f(ANCHO_TABLERO+ANCHO_BOTONES/2, 30.f), gui::style::clean);
    gui::button continuar("Continuar", myfont, sf::Vector2f(ANCHO_TABLERO+ANCHO_BOTONES/2, 80.f), gui::style::clean);
    gui::button mas_velocidad("+ Velocidad", myfont, sf::Vector2f(ANCHO_TABLERO+ANCHO_BOTONES/2, 130.f), gui::style::clean);
    gui::button menos_velocidad("- Velocidad", myfont, sf::Vector2f(ANCHO_TABLERO+ANCHO_BOTONES/2, 180.f), gui::style::clean);
    gui::button mas_zoom("+ Zoom", myfont, sf::Vector2f(ANCHO_TABLERO+ANCHO_BOTONES/2, 230.f), gui::style::clean);
    gui::button menos_zoom("- Zoom", myfont, sf::Vector2f(ANCHO_TABLERO+ANCHO_BOTONES/2, 280.f), gui::style::clean);
    gui::button limpiar("Limpiar", myfont, sf::Vector2f(ANCHO_TABLERO+ANCHO_BOTONES/2, 330.f), gui::style::clean);
    gui::button rellenar("Rellenar", myfont, sf::Vector2f(ANCHO_TABLERO+ANCHO_BOTONES/2, 380.f), gui::style::clean);
    gui::button btn_siguiente("Siguiente", myfont, sf::Vector2f(ANCHO_TABLERO+ANCHO_BOTONES/2, 430.f), gui::style::clean);
    gui::button guardar("Guardar", myfont, sf::Vector2f(ANCHO_TABLERO+ANCHO_BOTONES/2, 480.f), gui::style::clean);
    gui::button color_vivas("Color vivas", myfont, sf::Vector2f(ANCHO_TABLERO+ANCHO_BOTONES/2, 530.f), gui::style::clean);
    gui::button color_muertas("Color muertas", myfont, sf::Vector2f(ANCHO_TABLERO+ANCHO_BOTONES/2, 580.f), gui::style::clean);
    bool play=true;
    bool siguiente = false;
    int gen=0;
    //avanzar(std::vector<std::bitset<N>>& tablero, bool& play, int& delay){
    std::thread t(avanzar<ANCHO+2>, std::ref(tablero), std::ref(play), std::ref(delay));
    t.detach();
    while (window.isOpen())
    {   
        if(siguiente){
            siguiente=false;
            play=false;
        }
        window.setView(window.getDefaultView());
        sf::Event event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed){
                window.close();
            }
            if(event.type == sf::Event::MouseButtonPressed){
                if(event.mouseButton.x > ANCHO_TABLERO){
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
                    }
                    else if(event.mouseButton.y < 320.f ) { // zoom out
                        view_tablero.zoom(1.2);
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
                        guardar_tablero(tablero);
                    } else if(event.mouseButton.y < 570.f){ //color vivas
                        i_c_vivas=(i_c_vivas+1)%(sizeof(colores)/sizeof(*colores));
                        c_vivas = colores[i_c_vivas];
                    }else if(event.mouseButton.y < 620.f){ // color muertas
                        i_c_muertas=(i_c_muertas+1)%(sizeof(colores)/sizeof(*colores));
                        c_muertas = colores[i_c_muertas];
                    }
                } else{
                    int X = event.mouseButton.x/tam_celula;
                    int Y = event.mouseButton.y/tam_celula;
                    tablero[X][Y]=!tablero[X][Y];
                }
            }
            if(event.type == sf::Event::KeyPressed){
                switch (event.key.code)
                {
                case sf::Keyboard::A:
                    view_tablero.move(sf::Vector2f(-5.f, 0.f));
                    break;
                case sf::Keyboard::W:
                    view_tablero.move(sf::Vector2f(0.f, 5.f));
                    break;
                case sf::Keyboard::S:
                    view_tablero.move(sf::Vector2f(0.f, -5.f));
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
                default:
                    break;
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
        color_muertas.update(event, window);
        color_vivas.update(event, window);
        
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
        window.draw(color_muertas);
        window.draw(color_vivas);

        window.setView(view_tablero);
        for(int x = 0; x<ANCHO+1; x++){
            for(int y=0; y<ALTO+1; y++){
                sf::RectangleShape celula;
                celula.setPosition(x * tam_celula, y * tam_celula);
                celula.setSize(tam_vector);
                celula.setOutlineThickness(1);
                celula.setOutlineColor(gris);
                celula.setFillColor(tablero[x][y]?c_vivas:c_muertas);
                window.draw(celula);
            }
        }
        window.display();
        std::cout<<"Gen: "<<gen<<"\tVivas: "<<celulas_vivas(tablero)<<"\tShannon: "<<Shannon_entropy(tablero)<<"\n";
        gen++;

    }

    return 0;
}