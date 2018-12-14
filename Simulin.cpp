//Tarea 2 TEL[342] Luis Araya Barros 201330045-6

#include "scheduler.c"
#include "lcgrand.c"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include <math.h>

//std::random_device rd;
//std:: mt19937_64 seed (rd());

int seed = 18;

struct Usuario{
    int origen;
    int destino;
    int saltos;
    int FSU;
    float bloqueos;
    std::vector<int> ruta;
}usuario;

//definición de constantes
int nodos_Totales   = 0;
int enlaces_Totales = 0;
int max_hop         = 0;
int FSU             = 0;
float bloq_total    = 0;
float aten_total    = 0;
float carga_trafico = 0.3;
float t_on          = 0.001;
float t_off         = (t_on/carga_trafico) - t_on;

std::vector<Usuario> vector_Usuarios;
std::vector<std::vector<int>> matriz;

std::vector<std::vector<int>> crearMatriz(int columnas, int filas, int valor){
    std::vector<int> temp(filas, valor);
    std::vector<std::vector<int>> temp2(columnas, temp);
    return temp2;
}

void imprimeVectorDoble(std::vector<std::vector<int>> & vector1){
    std::vector<std::vector<int> >::iterator fila;
    std::vector<int>::iterator col;
    for (fila = vector1.begin(); fila != vector1.end(); fila++) {
        for (col = fila->begin(); col != fila->end(); col++) {
            std::cout<<*col<<" ";
        }
        std::cout<<'\n';
    }
}

float exp(float lambda, int s){
    return (-1/lambda)*(log(1-lcgrand(s)));
}

//función para seperar frases, cada palabra es parte de un vector
std::vector<std::string> splitString(std::string s){
    std::istringstream buf(s);
    std::istream_iterator<std::string> beg(buf), end;
    std::vector<std::string> tokens(beg, end);
    return tokens;
}


//función para obtener la tabla de rutas, cada ruta es una structura del tipo Rutas que contiene origen
//destino, saltos y camino, el camino es un vector de enteros donde cada uno representa la id del enlace
void getRutas(std::string s){

    //se abre el archivo y se guarda cada linea en un vector de strings, donde cada string es una 
    //linea del archivo, se eliminan las primeras 6 lineas ya que no aportan información relevante
    std::vector<std::string> v;
    std::string line;
    std::ifstream infile;
    infile.open(s);
    while(!infile.eof()){
        getline(infile, line);
        v.push_back(line);
    }
    infile.close();
    std::vector<std::string> temp = splitString(v[1]);
    nodos_Totales                 = std::stoi(temp[3]);
    temp                          = splitString(v[2]);
    enlaces_Totales               = std::stoi(temp[3]);
    std::vector<std::string>().swap(temp);
    v.erase(v.begin(),v.begin()+6);

    int aux = 0;

    //iterando sobre el vector creado anteriormente se procede a seperar cada string
    //para formar sub strings los cuales son origen, destino, saltos y camino, cada uno de estos sub strings
    //es guardado en el vector de rutas que finalmente representará la red completa 
    for(unsigned i = 0; i < v.size(); i++){
        std::vector<std::string> temp = splitString(v[i]);
        usuario.origen                = std::stoi(temp[0]);
        usuario.destino               = std::stoi(temp[1]);
        usuario.saltos                = std::stoi(temp[2]);
        if(usuario.saltos > aux)
            aux = usuario.saltos;
        if(temp.size() > 3){
            for(unsigned k = 3; k < temp.size(); k++){
                usuario.ruta.push_back(std::stoi(temp[k]));
            }
            vector_Usuarios.push_back(usuario);
            usuario.ruta.clear();
        }
        std::vector<std::string>().swap(temp);
    }
    max_hop = aux;
}


//se usa para el cálculo de métricas e imprimir lo que se pide
void printDatos(){
    //std::cout << "origen\tdestino\tbloqueo" << '\n';
    for(unsigned i = 0; i < vector_Usuarios.size(); i++){
        bloq_total = bloq_total + vector_Usuarios[i].bloqueos;
        std::cout << "\t" << vector_Usuarios[i].origen << "\t" << vector_Usuarios[i].destino 
        << "\t" << vector_Usuarios[i].bloqueos/(aten_total)
        << '\n';
    }
    std::cout << '\n' << "Bloqueo de Red:\t" << bloq_total/(aten_total) << '\n';
}

bool asignaFSU(int index, int req_FSU){
    int aux     = 1;
    int cont    = 0;
    int pos_FSU = 0;
    bool flag   = false;

    do{

        for(unsigned i = 0; i < vector_Usuarios[index].ruta.size(); i++){
            aux = aux * matriz[pos_FSU][i];
        }
        if(aux != 0){
            cont    = cont + 1;
        }
        pos_FSU = pos_FSU + 1;
    }while(cont != req_FSU || pos_FSU == matriz.size());

    if(cont == req_FSU){
        flag = true;
    }
    else{    
        return false;
    }

    vector_Usuarios[index].FSU = pos_FSU;
    for(int i = pos_FSU; i > (pos_FSU-req_FSU); i--){
        for(unsigned j = 0; j < vector_Usuarios[index].ruta.size(); j++){
            matriz[i][j] = 0;
        }
    }
    return flag;
}

void liberaFSU(int index, int req_FSU){
    int pos_FSU = vector_Usuarios[index].FSU;
    for(int i = pos_FSU; i > (pos_FSU-req_FSU); i--){
        for(unsigned j = 0; j < vector_Usuarios[index].ruta.size(); j++){
            matriz[i][j] = -1;
        }
    }
}

//lógica de conexión
void conectar(int index, float t){
    int req_FSU = vector_Usuarios[index].ruta.size();
    bool flag = asignaFSU(index, req_FSU);
    //si hay recursos se genera un evento de conmutación del usuario
    if(flag){
        pushEvento(crearEvento(1, index, t+exp(1/t_on, seed)));
    }
    //si no hay recursos se registra el bloqueo y se genera un nuevo evento de conexión
    else{
        vector_Usuarios[index].bloqueos = vector_Usuarios[index].bloqueos + 1;
        pushEvento(crearEvento(0, index, t+exp(1/t_off, seed)));
    }
}

//lógica de conmutación
void conmutar(int index, float t){
    int req_FSU = vector_Usuarios[index].ruta.size();
    //se liberan los recursos utilizados
    liberaFSU(index, req_FSU);
    //se crea un nuevo evento de conexión
    pushEvento(crearEvento(0, index, t+exp(1/t_off, seed)));
}

//simulación completa
void sim(float ll){
    //se obtiene el vector de rutas
    long cont = 0;
    //inicialización del scheduler con eventos de conexión para cada usuario
    for(unsigned i = 0; i < vector_Usuarios.size(); i++){
        pushEvento(crearEvento(0, i, exp((1/t_on), seed)));
    }
    
    //bloque de simulación, se saca un evento del scheduler y se procesa
    while(cont < ll){
        popEvento();
        if(pScheduler->tipo == 0){
            conectar(pScheduler->index, pScheduler->tiempo);
        }
        if(pScheduler->tipo == 1){
            conmutar(pScheduler->index, pScheduler->tiempo);    
        }
        cont = cont + 1;
        free(pScheduler);
    }
    aten_total = ll;
    //printDatos();
}

int main(int argc, char const *argv[]){
    std::string file = argv[1];
    float llegadas = std::stoi(argv[2]);
    FSU = std::stoi(argv[3]);
    getRutas(file);
    matriz = crearMatriz(FSU, enlaces_Totales, -1);
    sim(llegadas);
    //imprimeVectorDoble(matriz);
    return 0;
}