#include <SFML/Graphics.hpp>
#include "button.hpp"
#include <iostream>
#include <bitset>
#include <vector>
#include <thread>
#include <mutex>
#include <math.h>
#include <random>

#define ANCHO_TABLERO 900
#define ANCHO_GRAFICAS 400
#define ALTO_VENTANA 700
#define TAM 900

std::mutex mu;
double porcentaje=0.4;
unsigned char configuracion = 30;
const int n_datos = ANCHO_GRAFICAS/5;
std::vector<float> shannon(n_datos);
std::vector<float> logaritmos(n_datos);
std::vector<int>  num_celulas(n_datos);
const sf::Color gris(166,166,166), purple(166, 62, 197), teal(57, 174, 169), teal_black(6, 44, 48);

template<size_t N>
void llenarTablero(std::bitset<N> &tablero){
    int celulas_vivas_inicio = N*porcentaje;
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> X(0, N-1);
    while(celulas_vivas_inicio--){
        int x = X(gen);
        if(tablero[x])
            celulas_vivas_inicio++;    
        tablero[X(gen)]=true;
    }
}

template<size_t N>
void limpiarTablero(std::bitset<N> &tablero){
    tablero.reset();
}


int mod(int a, const int m){
    a%=m;
    return a<0?a+m:a;
}

template<size_t N>
long long int celulas_vivas(std::bitset<N> &tablero){
    return tablero.count();
}

template<size_t N>
double Shannon_entropy(const std::bitset<N> &tablero){
    int conf=0;
    std::vector<int> Q_i(8,0);
    for(size_t i = 0; i<N; i++){
        conf=0;
            for(int a=-1; a<=1; a++){
                conf|=tablero[mod(i+a, N)]<<(a+1);
            }
        Q_i[conf]++;
    }      
    double res = 0;
    for(auto q : Q_i){
        if(q!=0){
            const double q_n = q/((double) N);
            res+=(q_n)*log2(q_n);
        }
    }
    return -res;
    
}

template<size_t N>
double logaritmo(std::bitset<N> &fila){
    return log10(fila.count());
}

template<size_t N>
void avanzar(std::bitset<N> &fila, bool &play, int &delay, int &gen, std::vector<std::bitset<N>> &historial){
    while(true){
    if(play){
        mu.lock();
        gen++;
        std::bitset<N> anterior(fila);
        for(int i = 0; i<N; i++){
            fila[i]=(configuracion>>(anterior[mod(i-1, N)]<<2 | anterior[i]<<1 | anterior[mod(i+1, N)]))&1;
        }
        historial.push_back(fila);
        
        if(gen<n_datos){
            num_celulas[gen]=celulas_vivas(fila);
            shannon[gen]=Shannon_entropy(fila);
            logaritmos[gen]=logaritmo(fila);
        } else{
            std::rotate(num_celulas.begin(), num_celulas.begin()+1, num_celulas.end());
            std::rotate(shannon.begin(), shannon.begin()+1, shannon.end());
            std::rotate(logaritmos.begin(), logaritmos.begin()+1, logaritmos.end());
            num_celulas[n_datos-1]=celulas_vivas(fila);
            shannon[n_datos-1]=Shannon_entropy(fila);
            logaritmos[n_datos-1]=logaritmo(fila);
        }
        mu.unlock();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}


void menu(){
    const std::string menu_s = "1)Cambiar densidad\n2)Cambiar regla\nOpción: ";
    while(true){
        std::cout<<menu_s;
        int opc=0;
        std::cin>>opc;
        switch (opc){
        case 1:
            std::cout<<"Ingrese la densidad en un rango de 0.0 - 1.0: ";
            std::cin>>porcentaje;
            break;
        case 2:
            int conf_aux;
            std::cout<<"Ingrese el número de regla(0-255): ";
            std::cin>>conf_aux;
            if(conf_aux>=0 && conf_aux<=255){
                configuracion=conf_aux;
            } else{
                std::cout<<"Regla fuera del rango\n";
            }
            break;
        default:
            break;
        }
    }
}

int main(){
    std::bitset<TAM> fila;
    std::vector<std::bitset<TAM>> historial;
    fila[1]=1;
    sf::RenderWindow window(sf::VideoMode(ANCHO_TABLERO+ANCHO_GRAFICAS, ALTO_VENTANA), "AC 1 dimension");
    window.setPosition(sf::Vector2i(50, 50));
    const int tam_celula = 1;//ANCHO_TABLERO/TAM;
    const sf::Vector2f tam_vector(tam_celula, tam_celula);
    const float ancho_total = ANCHO_TABLERO + ANCHO_GRAFICAS;

    sf::Color colores[] = {sf::Color::Black, sf::Color::Cyan, sf::Color::Green,
        sf::Color::Magenta, sf::Color::White, sf::Color::Red, sf::Color::Yellow, 
        sf::Color::Blue, purple, teal, teal_black};
    int8_t i_c_vivas=0, i_c_muertas=4;
    sf::Color c_vivas=colores[i_c_vivas], c_muertas=colores[i_c_muertas];
    sf::View view_tablero(sf::FloatRect(0.f, 0.f, static_cast<float>(TAM), static_cast<float> (ALTO_VENTANA)));
    view_tablero.setViewport(sf::FloatRect(0.f, 0.f, ANCHO_TABLERO/ancho_total, 1.f));
    sf::View view_graficas(sf::FloatRect(static_cast<float>(ANCHO_TABLERO), 0.f, static_cast<float>(ANCHO_GRAFICAS), static_cast<float> (ALTO_VENTANA)));
    view_graficas.setViewport(sf::FloatRect((ANCHO_TABLERO)/ancho_total, 0.f, ANCHO_GRAFICAS/ancho_total, 1.f));
    sf::Font myfont;if(!myfont.loadFromFile("MesloLGS.ttf")){
        std::cerr<<"Could not find contb.ttf font."<<std::endl;
    }
    gui::button btn_poblacion("POBLACION: ", myfont, sf::Vector2f(ANCHO_TABLERO+ANCHO_GRAFICAS/2, 25), gui::style::cancel);
    gui::button btn_shannon("SHANNON: ", myfont, sf::Vector2f(ANCHO_TABLERO+ANCHO_GRAFICAS/2, 255), gui::style::cancel);
    gui::button btn_logaritmo("LOGARITMO: ", myfont, sf::Vector2f(ANCHO_TABLERO+ANCHO_GRAFICAS/2, 485), gui::style::cancel);
    gui::button btn_gen("GENERACION: ", myfont, sf::Vector2f(ANCHO_TABLERO+ANCHO_GRAFICAS/2, 675.f), gui::style::cancel);
    bool play = true;
    bool siguiente = false;
    int delay = 100;
    int gen=0;
    std::thread t(avanzar<TAM>, std::ref(fila), std::ref(play), std::ref(delay), std::ref(gen), std::ref(historial));
    t.detach();
    std::thread t_menu(menu);
    t_menu.detach();
    bool cambio = true;
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
                cambio=true;
            }
        }
        window.setView(view_graficas);
        sf::RectangleShape rectNegro;
        rectNegro.setPosition(ANCHO_TABLERO, 0);
        rectNegro.setSize(sf::Vector2f(ANCHO_GRAFICAS, ALTO_VENTANA));
        rectNegro.setFillColor(sf::Color::Black);
        window.draw(rectNegro);
        btn_shannon.update(event, window);
        btn_poblacion.update(event, window);
        btn_logaritmo.update(event, window);
        btn_gen.update(event, window);
        mu.lock();
        btn_gen.setText("GENERACION: " + std::to_string(celulas_vivas(fila)));
        if(gen<n_datos){
            btn_shannon.setText("SHANNON: " + std::to_string(shannon[gen]));
            btn_poblacion.setText("POBLACION: " + std::to_string(num_celulas[gen]));
            btn_logaritmo.setText("LOGARITMO: " + std::to_string(logaritmos[gen]));
        }
        else{
            btn_shannon.setText("SHANNON: " + std::to_string(shannon[n_datos-1]));
            btn_poblacion.setText("POBLACION: " + std::to_string(num_celulas[n_datos-1]));
            btn_logaritmo.setText("LOGARITMO: " + std::to_string(logaritmos[n_datos-1]));
        }
        
        window.draw(btn_shannon);
        window.draw(btn_poblacion);
        window.draw(btn_logaritmo);
        window.draw(btn_gen);
        for(int i = 1; i<n_datos; i++){
            if(i>gen)
                break;
            sf::Vertex line1[]={
                sf::Vertex(sf::Vector2f(ANCHO_TABLERO +(i-1)*5, 230 - num_celulas[i-1]/3.5f)), 
                sf::Vertex(sf::Vector2f(ANCHO_TABLERO + i*5, 230 - num_celulas[i]/3.5f))
            };
            window.draw(line1, 2, sf::Lines);
            sf::Vertex line2[]={
                sf::Vertex(sf::Vector2f(ANCHO_TABLERO + (i-1)*5, 460-shannon[i-1]*40)), 
                sf::Vertex(sf::Vector2f(ANCHO_TABLERO + i*5, 460 - shannon[i]*40))
            };
            window.draw(line2, 2, sf::Lines);
            sf::Vertex line3[]={
                sf::Vertex(sf::Vector2f(ANCHO_TABLERO + (i-1)*5, 650-logaritmos[i-1]*30)), 
                sf::Vertex(sf::Vector2f(ANCHO_TABLERO + i*5, 650 - logaritmos[i]*30))
            };
            window.draw(line3, 2, sf::Lines);
        }

        window.setView(view_tablero);
        if(cambio || gen<5){
            window.clear();
            sf::RectangleShape muertas;
            muertas.setPosition(0, 0);
            muertas.setSize(sf::Vector2f(ANCHO_TABLERO*tam_celula, ALTO_VENTANA*tam_celula));
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
        } else{
            for(int y=historial.size()-1; y>=historial.size()-3; y--) //solo 3 generaciones anteriores
            for(int x = 0; x<TAM; x++){
                if(historial[y][x]){
                    sf::RectangleShape celula;
                    celula.setPosition(x * tam_celula, y * tam_celula);
                    celula.setSize(tam_vector);
                    celula.setFillColor(c_vivas);//fila[x]?c_vivas:c_muertas);
                    window.draw(celula);
                }
            }
        }
        mu.unlock();
        cambio=false;
        window.display();
    }
}