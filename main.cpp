#include<bits/stdc++.h>

using namespace std;

template<size_t N>
void llenarTablero(vector<bitset<N>> &tablero, double porcentaje){
    int celulas_vivas_inicio = N*N*porcentaje;
    random_device rd;  
    mt19937 gen(rd()); 
    uniform_int_distribution<> X(0, N-1);
    uniform_int_distribution<> Y(0, N-1);
    while(celulas_vivas_inicio--){
        int x = X(gen), y = Y(gen);
        if(tablero[x][y])
            celulas_vivas_inicio++;    
        tablero[X(gen)].set(Y(gen));
    }
}
template<size_t N>
int cuenta(vector<bitset<N>> &tablero, int I, int J){
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
void avanzar(vector<bitset<N>> &tablero){
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
long long int celulas_vivas(vector<bitset<N>> &tablero){
    long long int cnt = 0;
    for(auto bits : tablero){
        cnt+=bits.count();
    }
    return cnt;
}

int main(){
    /*ofstream salida;
    salida.open ("salida2.csv");
    salida<<"Porcentaje;Promedio;\n";
    for(double porcentaje = 0.00; porcentaje<=1.00; porcentaje+=0.01){
        long long int contadas=0;
        for(int j = 0; j<10; j++){
            vector<bitset<10000>> tablero(10000);
            llenarTablero(tablero, porcentaje);
            for(int i = 0; i<30; i++){
                avanzar(tablero);
            }    
            contadas += celulas_vivas(tablero);
        }
        double promedio = contadas/10;
        //salida<<porcentaje<<";"<<promedio<<";\n";
        cout<<promedio<<endl;
    }

    salida.close();
    */

    vector<bitset<1000>> tablero(1000);
    llenarTablero(tablero, 0.5);
    while(true){
        avanzar(tablero);
        cout<<tablero[500][500]<<endl;
    }

    return 0;


}