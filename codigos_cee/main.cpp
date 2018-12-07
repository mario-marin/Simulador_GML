#include <cstdlib>
#include <iostream>
#include "lcgrand.c"
#include "scheduler.c"
#include <cmath>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
using namespace std;
void pushEvento(Evento* pEvento);             //Se define el struct porque no usare el scheduler.h (ni tampoco el lcgrand.h) para hacer la compilacion en una linea 
Evento *crearEvento(int tipo,float tiempo, int idx);
Evento *popEvento();
void imprimirScheduler();
int schedulerIsEmpty();
float i_exp(float p, float lamda);
float gt_evento(float lamda);
float numero_rep(int i);
void evento_simple(Evento *nevento, float tiempo, int tipo, int id);
void poblamiento(int ultimo, Evento *nevento,float lamda, int tipo, float t_actual);
void agregar_evento_exp(Evento *neventoX, float lamdaX, int tipoX, float t_actual, int idX);
void imprime_vector_doble(vector <vector<int>> &vector1);
void imprime_vector_doble_float(vector <vector<float>> &vector1);
void imprime_vector_simple(vector <int> &vector1);
void imprime_estado_path(vector <int> &vector_path, vector <int> &vector_general);
int link_puede_enviar(int capacidad, int users_actual);
int path_disponible(vector <int> &vector_path, vector <int> &vector_general, int C);
void incremento_user_en_path(vector <int> &links_de_user, vector<int> &links_general);
void disminucion_user_en_path(vector <int> &links_de_user, vector<int> &links_general);
void prob_bloqueo_por_usuario(vector<int> &user_actual1 , vector <vector<float>> &probs, int id);
int largo_vector(vector <int> &vector1);
void historial_user(vector <int> &user_actual, int io);
float promedio(float x1, float x2);
double avance(unsigned int intentos,Evento *nevento,float lamda,float miu,int C,vector <vector<int>> &rutas,vector <int> &links,vector <vector<float>> &probs_por_user,int* largo_max);
vector<vector<int>> ingresa_datos(string txt, int* max, int* idmax); 
vector <int> crea_vector(int valor_inicial, int largo);
vector <float> crea_vector_float(float valor_inicial, float largo);
vector<vector<float>> crea_vector_doble(float largo, float relleno, float cantidad_rellenos);
void promedios(vector<vector<float>> &prob, vector <float> &prob_largo);
void imprime_vector_simple_float(vector <float> &vector1);
int busca_menor_intento(vector <vector<int>> &vector1);

string deja_nombre(string str, int n){
    int cont=0;
    while(str.size()>0 && cont<=n){
        str.pop_back();
        ++cont;
    }
    return str;
}

int main() {    
    string n1 = "ArpaNet_f.rut";
    string n2 = "EON_f.rut";
    string n3 = "EuroCore_f.rut";
    string n4 = "EuroLarge_f.rut";
    string n5 = "NSFNet_f.rut";
    string n6 = "UKNet_f.rut";
    string nombre;
    
    int cap0n1=27; //done
    int cap0n2=17; //done
    int cap0n3=4;  //done
    int cap0n4=61; //done
    int cap0n5=13; //done
    int cap0n6=19; //done
    
    
   unsigned int llegadas = 10000000; /*llegadas es el numero total de intentos de uso de de los caminos por usuario, independiente si logra transmitir o no.
   se puede hacer con mas, pero  un millon de llegadas ya hace que se demore como 1 segundo y como hay que iterar hasta un orden de error de 
   10^-3 prefiero hacerlo con 150mil */  
   Evento *nevento;
   int max, idmax, largo_max;
  
   
   float ton = 0.001;
   float gamma = 0.3;
   float toff = (ton/gamma)-ton;
   float lambda = 1/(ton+toff);
   float miu = 1/toff;
   double prob_acepto, prob_rechazo;
   vector <float> avg_prob_por_largo;
   vector <vector<float> > prob_bpu; //probabilidad de bloqueo por usuario
    
   nombre=n1;                             ///////////////////////////////////////////////////////////////////////////////////////////
   int c_v = cap0n1; //capacidad_variable ///////////////////////////////////////////////////////////////////////////////////////////
   
   vector<vector<int> > rutas = ingresa_datos(nombre, &max, &idmax); 
   
    poblamiento(idmax, nevento,3,1,0);
    float largo_float=(float)buscarmaxid(); 
    prob_bpu=crea_vector_doble(largo_float,0,2);  //vector de probabilidades de rechazo por usuario
    vector <int> enlaces = crea_vector(0, max+1); 
    
    string aux=deja_nombre(nombre,5);
    string datos1=aux+"_por_capacidad.txt";
    string datos2=aux+"_por_largo.txt";
    
    ofstream datos_capacidad;    //Se mandan los archivos a un .txt
    datos_capacidad.open(datos1);
    
    
    
    do{
    cout<<"Capacidad test: "<<c_v<<endl;
    prob_rechazo=avance(llegadas, nevento,lambda,miu,c_v,rutas,enlaces,prob_bpu,&largo_max); 
    --c_v; 
    cout<<"La probabilidad de rechazo es: "<<prob_rechazo<<endl;
    
    datos_capacidad<<c_v<<" "<<prob_rechazo<<endl;
    }while(prob_rechazo<=0.001 && c_v>=0);
    datos_capacidad.close();
    
    
    prob_acepto=1-prob_rechazo;
    
    avg_prob_por_largo=crea_vector_float(0,largo_max+1);
   
    promedios(prob_bpu, avg_prob_por_largo); //funcion que calcula promedio de bloqueo por largo de enlace y lo almacena en otro vector
    //imprime_vector_simple_float(avg_prob_por_largo);
    
    ofstream datos_largo;
    datos_largo.open(datos2);
    int cont_aux = 0;
    vector <float>::iterator linea;
    for(linea=avg_prob_por_largo.begin(); linea!=avg_prob_por_largo.end(); linea++){
        cout<<"Largo: "<<cont_aux<<" Prob bloqueo: "<<*linea<<endl;
        datos_largo<<cont_aux<<" "<<*linea<<endl;
        ++cont_aux;
    }
    datos_largo.close();
    
    
    
    
    return 0;
}

//www.stackoverflow.com
//https://stackoverflow.com/questions/14516915/read-numeric-data-from-a-text-file-in-c -> leeerdata del txt
//www.geeksforgeeks.org -> sacas numeros de los str
//https://stackoverflow.com/questions/22655059/why-it-is-ok-to-return-vector-from-function   wea util


float i_exp(float p, float lamda){ //funcion para generar numeros exponenciales
    return -(1/lamda)*log(1-p);
}

float gt_evento(float lamda){  //generar tiempo de un evento enesimo
        return i_exp(lcgrand(50), lamda);
}

float numero_rep(int i){
    return pow(10,i*1.0);
}

void evento_simple(Evento *nevento, float tiempo, int tipo, int id){
    nevento=crearEvento(tipo,tiempo,id);
    pushEvento(nevento);
    return;
}

void poblamiento(int ultimo, Evento *nevento,float lamda, int tipo, float t_actual){
    int rep_cont = 0;
    do{
        float evx = gt_evento(lamda)+t_actual;
        if(evx>0){
            evento_simple(nevento, evx, tipo, rep_cont);
            rep_cont = rep_cont + 1;
        }
        
    }while(rep_cont <= ultimo);
    
    return;
}

void agregar_evento_exp(Evento *neventoX, float lamdaX, int tipoX, float t_actual, int idX){
    float xyz=gt_evento(lamdaX);
    float tiempoX = xyz+t_actual;
    evento_simple(neventoX, tiempoX, tipoX, idX);
    return;
}

void imprime_vector_doble(vector <vector<int>> &vector1){
    vector< vector<int> >::iterator fila;
    vector<int>::iterator col;
    for (fila = vector1.begin(); fila != vector1.end(); fila++) {
        for (col = fila->begin(); col != fila->end(); col++) {
            cout<<*col<<" ";
        }
        cout<<endl;
    }
    return;
}

int busca_menor_intento(vector <vector<int>> &vector1){
    int aux = 1000000000;
    vector < vector<int> >::iterator fila;
    vector<int>::iterator col;
    for (fila = vector1.begin(); fila != vector1.end(); fila++) {
        col=fila->begin();
        if(*col<=aux && *col>0){
            aux=*col;
        }
    }
    return aux;
}

void imprime_vector_doble_float(vector <vector<float>> &vector1){
    vector< vector<float> >::iterator fila;
    vector<float>::iterator col;
    for (fila = vector1.begin(); fila != vector1.end(); fila++) {
        for (col = fila->begin(); col != fila->end(); col++) {
            cout<<*col<<" ";
        }
        cout<<endl;
    }
    return;
}

void imprime_vector_simple(vector <int> &vector1){
    vector <int>::iterator dato;
    for(dato=vector1.begin(); dato!=vector1.end(); dato++){
        cout<<*dato<<" ";
    }
    return;
}

void imprime_vector_simple_float(vector <float> &vector1){
    int i = 0;
    vector <float>::iterator dato;
    for(dato=vector1.begin(); dato!=vector1.end(); dato++){
        cout<<"Largo: "<<i<<" Prob bloqueo: "<<*dato<<endl;
        ++i;
    }
    return;
}

void imprime_estado_path(vector <int> &vector_path, vector <int> &vector_general){
    vector <int>::iterator link;
    for(link=vector_path.begin(); link!=vector_path.end(); link++){
        cout<<"link: "<<*link<<" Estado: "<<vector_general[*link]<<" ";
    }
    return;
}

int link_puede_enviar(int capacidad, int users_actual){ //puede enviar ssi tiene almenos 1 slot
    if(users_actual<capacidad){
        return 1;
    }
    else{
        return 0;
    }
}

int path_disponible(vector <int> &vector_path, vector <int> &vector_general, int C){
    vector <int>::iterator link;
    int aux;
    int x = 1;
    for(link=vector_path.begin()+6; link!=vector_path.end(); link++){
        aux=link_puede_enviar(C, vector_general[*link]);
        x=x*aux;
    }
    return x;
}

void incremento_user_en_path(vector <int> &links_de_user, vector<int> &links_general){
    vector <int>::iterator link;
    for(link=links_de_user.begin()+6; link!=links_de_user.end(); link++){
        ++links_general[*link];    
    }
    return;
}

void disminucion_user_en_path(vector <int> &links_de_user, vector<int> &links_general){
    vector <int>::iterator link;
    for(link=links_de_user.begin()+6; link!=links_de_user.end(); link++){
        --links_general[*link];    
    }
    return;
}

void prob_bloqueo_por_usuario(vector<int> &user_actual1 , vector <vector<float>> &probs, int id){
    float total = (float)user_actual1[0];
    float rechazos = (float)user_actual1[2];
    float largo = (float)user_actual1[5];
    if(total==0 || rechazos==0){
        probs[id][0]=0;
    }
    else{
        probs[id][0]=(rechazos*1.0)/total;    
    }
    probs[id][1]=largo;
    return;
}

int largo_vector(vector <int> &vector1){
    return static_cast<int>(vector1.size());
}

void historial_user(vector <int> &user_actual, int io){
    if(io==1){  //se acepto
        ++user_actual[1];
    }
    else if(io==0){  //se rechazo
        ++user_actual[2];
    }
    ++user_actual[0];
    return;
}

float promedio(float x1, float x2){
    return ((x1+x2)/2);
}

double avance(unsigned int intentos,Evento *nevento,float lamda,float miu,int C,vector <vector<int>> &rutas,vector <int> &links,vector <vector<float>> &probs_por_user,int* largo_max){  
    unsigned int i;
    double rechazos = 0;
    int id;
    float tiempo;
    int tipo, largo_v, v_actual_valido, path_valido;
    vector <int> user_actual;
    *largo_max=-1;
    for(i=0; i<=intentos; ++i){              
        nevento=popEvento(); //tommamos el nodo actual y pasa a eliminarse
        id=nevento->id;
        tiempo=nevento->tiempo;
        tipo=nevento->tipo; 
        user_actual=rutas[id]; //tomamos los links del user actual en un vector llamaso user_actual
        largo_v=largo_vector(user_actual);
        if(*largo_max<=user_actual[5]){
            *largo_max=user_actual[5];
        }
        
        if(largo_v>6){
            if(tipo==1){//quiere iniciar Tx
                path_valido=path_disponible(user_actual, links, C); //comprobamos que se pueda usar ese camino
                if(path_valido==1){ //puede transmitir
                    agregar_evento_exp(nevento,miu,2, tiempo, id); //agenda evento salida
                    incremento_user_en_path(user_actual, links);
                    historial_user(rutas[id],1);
                    
                }
                else if(path_valido==0){ //no puede transmitir
                    agregar_evento_exp(nevento,lamda,1, tiempo, id); //agenda evento de entrada
                    historial_user(rutas[id],0);
                    rechazos++;
                }
            }
            if(tipo==2){ //quiere apagar tx
                agregar_evento_exp(nevento,lamda,1, tiempo, id);
                disminucion_user_en_path(user_actual, links);
            }
        prob_bloqueo_por_usuario(user_actual , probs_por_user, id);
        }
        
        free(nevento);
    }
    float probx=abs(rechazos/((double)intentos));
    if(probx>1){ //si por algun extranio motivo desconocido
        return 1;
    }
    else{
        return probx;
    }
    
}

vector<vector<int>> ingresa_datos(string txt, int* max, int* idmax){
    *idmax=0;
    *max=-1;
    int i = 0;
    ifstream datos(txt);
    vector<vector<int> > numbers;
    string temp;
    int aux;
        while (getline(datos, temp)) {
        istringstream buffer(temp);
        vector<int> line((istream_iterator<int>(buffer)), istream_iterator<int>());
        aux = *max_element(line.begin(), line.end());
        line.insert(line.begin(),0); 
        line.insert(line.begin(),0);
        line.insert(line.begin(),0);
        numbers.push_back(line);
        if(aux>*max){
            *max=aux;
        }
        i++;
    }
    *idmax = i;
    return numbers;
} 

vector <int> crea_vector(int valor_inicial, int largo){
    vector <int> vector1(largo, valor_inicial); 
    return vector1;
}

vector <float> crea_vector_float(float valor_inicial, float largo){
    vector <float> vector1(largo, valor_inicial); 
    return vector1;
}

vector<vector<float>> crea_vector_doble(float largo, float relleno, float cantidad_rellenos){
    vector <float> linea(cantidad_rellenos, relleno);
    vector <vector<float>> vector1(largo, linea);
    return vector1;
}

void promedios(vector<vector<float>> &prob, vector <float> &prob_largo){
    vector< vector<float> >::iterator fila;
    vector<float>::iterator col;
    float prom, largo;
    for (fila = prob.begin(); fila != prob.end(); fila++) {
        col=fila->begin();
        prom=*col;
        ++col;
        largo=*col;
        prob_largo[largo]=promedio(prob_largo[largo],prom);       
    }
    return;
}