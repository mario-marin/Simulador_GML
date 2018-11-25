#include <iostream>
#include <math.h>
#include <random>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "lcgrand.c"
#include "scheduler.c"


using namespace std;

//////////////////////--global variables--//////////////////////////////////////////
float usage;                //numero de clientes en el canal
double blocked;             //numero total de blockeos
double total_arrivals = 0;  //numero total de arrivos
vector<vector<int>> links;  //links         [links_id]->[capacidad,uso]
vector<vector<int>> users;  //usuarios      [usuario_id]->[llegadas,blokeo,#hops,the_hops]

random_device rd;
mt19937_64 seed (rd());

//////////////////////--funtions--//////////////////////////////////////////////////

float probability(float lambda){ //calculo de tiempo exponencial
    return (-1.0/lambda)*log(1-lcgrand(seed()%100)); // probabilidad exponecial
}

int arrive_user(int id_user, float lambda,float mu, float sim_time){
    float prob_temp;
    total_arrivals++;        // se aumenta las llegadas totales
    bool enable_flag = true; // flag indica si todos los enlases del usuario estan disponibles

    users[id_user][0] = users[id_user][0] + 1;      // se aumenta las llegadas del usuario entrante

    for (int i = 0; i < users[id_user][2]; ++i) {   // se revisa si los enlases ocupados por el usuario se encuentran disponibles
        if (links[users[id_user][3+i]][0] > links[users[id_user][3+i]][1] ) {// revision de capacidad
            continue;
        } else {
            enable_flag = false; //si almenos un enlase reporta no tener suficiente capacidad el usuario es bloqueado
            break;
        }
    }

    if (enable_flag == true ) {                      // si el for anterior no reporto enlases no-disponibles, entonses se procede con la reservacion del enelase
        for (int i = 0; i < users[id_user][2]; ++i) {// se aumenta el uso del link extrallendo su id desde el usuario
            links[users[id_user][3+i]][1] = links[users[id_user][3+i]][1] + 1;
        }
        prob_temp = probability(mu);              //calculo de tiempo de salida
        pushEvento(crearEvento(0,id_user,prob_temp+sim_time)); //se agrega al scheduler
        return 100;
    } else {                                      // usuario bloqueado
        blocked++;                                // se aumenta los usuarios bloqueados totales
        users[id_user][1] = users[id_user][1] + 1;// se aumentan el contador de bloqueos del usuario
        prob_temp = probability(lambda);          // calculo de tiempo de siguiente llegada
        pushEvento(crearEvento(1,id_user,prob_temp+sim_time)); //se agrega al scheduler
        return 404;
    }
}

int exit_user(int id_user, float lambda, float sim_time){
     float prob_temp;
     for (int i = 0; i < users[id_user][2]; ++i) { //se libera el espacio ocupado por el usuario en todos los enlases
         links[users[id_user][3+i]][1] = links[users[id_user][3+i]][1] - 1;
     }
     prob_temp = probability(lambda);                           //calculo de tiempo de llegada
     pushEvento(crearEvento(1,id_user,prob_temp+sim_time)); //se agrega al scheduler
     return 0;
}

void print_results(int max_hops){//funcion que imprime datos para desplegar en jupyter
    vector<vector<float>> stats;// [cada indise es un largo de hops partiendo en 1]->[promedio de probabilidades,numero de ocurrencias]
    stats.resize(max_hops);
    for (unsigned int i = 0; i < stats.capacity(); ++i) { // inicialisacion del vector generado.
        stats[i].resize(2);
        stats[i][0] = 0;
        stats[i][1] = 0;
    }
    for (unsigned int i = 0; i < users.capacity(); ++i) {
        stats[users[i][2]-1][0] = (float)users[i][1]/(float)users[i][0] + stats[users[i][2]-1][0]; // suma de probablidades con el mismo largo
        stats[users[i][2]-1][1] = stats[users[i][2]-1][1] + 1;                                     //cantidad de sumas para calcular el promedio
    }
    for (unsigned int i = 0; i < stats.capacity(); ++i) {// se imprime las probabilidades de bloqueo optenidos de cada usuario
        cout << stats[i][0]/stats[i][1] << endl;
    }
    cout << blocked/total_arrivals << endl;             // se entrega el prob de blokeo de la red completa
}

////////////////////////////--code--///////////////////////////////////////


int main(int argc, char *argv[])  // argumentos : nombre_de_red, capcidad de enlases, nomero_de_llegadas_de_parada, t_on, carga_promedio
{

    int temp;
    string file = argv[1];
    string rut = file+".rut";
    int C = atoi(argv[2]);
    long int LLEGADAS = atoi(argv[3]);  //criterio de finalizasion
    float ton = atof(argv[4]);
    float carga = atof(argv[5]);
    float toff = (ton-carga*ton)/carga;


    float lambda = 1.0/(ton+toff);
    float mu = 1.0/(ton);


    ifstream infile("./Redes\ y\ Rutas/Rutas/"+rut);
    string line;
    string temp_str;
    int counter=0;

    int user_counter = 0;
    int node_1;
    int node_2;
    int load_status = 0;
    int hops = 0;
    int max_hops = 0;
    int hop_counter = 0;
    int init_flag=0;

    //-------------------routes parsing---------------------
    while(getline(infile,line,'\t')){
        istringstream stream(line);
        while(getline(stream,temp_str,'\n')){
            if(temp_str=="\r"){
                continue;
            }
            if (counter==7) {         // se extrae la cantidad de usuarios totales
                temp = stoi(temp_str);
                users.resize(temp*(temp-1));
            }else if (counter == 11) {// se extrae la cantidad de enlases totales
                temp = stoi(temp_str);
                links.resize(temp);
            }else if (counter>19){    // se comoensa a estraer la informacion del archivo (no header)
                if (load_status==0) { // se extrae el nodo_1
                    node_1 = stoi(temp_str);
                    load_status++;
                } else if (load_status==1) {// se extrae el nodo_2
                    node_2 = stoi(temp_str);
                    load_status++;
                } else if (load_status==2 && (node_1!=node_2)) {// se cargan datos validos NO LOOPBACK
                    if (init_flag == 0) {  // se cargan el numero de hops y se configura el vector de forma adecuada
                        init_flag = 1;
                        hops = stoi(temp_str);
                        users[user_counter].resize(hops+3);
                        users[user_counter][0] = 0;
                        users[user_counter][1] = 0;
                        users[user_counter][2] = hops;
                        if (max_hops<hops) {
                            max_hops = hops;
                        }

                    } else {
                        if (hops-1 > hop_counter) {//se cargan los hops
                            hop_counter++;
                            users[user_counter][2+hop_counter] = stoi(temp_str);

                        } else if (hops-1 == hop_counter){
                            hop_counter++;
                            users[user_counter][2+hop_counter] = stoi(temp_str);
                            load_status = 0;
                            hops = 0;
                            hop_counter = 0;
                            init_flag=0;
                            user_counter++;
                        } else {// error al cargar los datos
                            cout  << "ERROR WHILE LOADING RUT FILE" << endl;
                        }
                    }
                } else {
                    load_status = 0;
                }
            }
            counter++; // se procede con el siguiente caracter
        }
    }
    for (unsigned int i = 0; i < links.capacity(); ++i) { // se inicialisa el vector de links
        links[i].resize(2);
        links[i][0] = C;        //capacidad
        links[i][1] = 0;        //uso
    }

    //-----------------end load---------------------------

    double sim_time = 0;
    Evento * evento_temp;
    float prob_temp;


    //-----------------init block--------------------------

    for (unsigned int i = 0; i < users.capacity(); ++i) {// se unizialisa la simulacion
        prob_temp = probability(lambda);
        pushEvento(crearEvento(1,i,prob_temp));          //1 is on
    }
    //-----------------simulation--------------------------

    while (LLEGADAS > total_arrivals) {     //simulation con condicion de parada

        evento_temp = popEvento();

        if(sim_time != evento_temp->tiempo){//se verifica que el evento extraido no ocurrio al mismo tiempo que el evento anterior (coliciones)
            sim_time = evento_temp->tiempo;
        }

        if(evento_temp->tipo == 1){         //se verifica si es una llegada o salida
            arrive_user(evento_temp->usuario,lambda,mu,sim_time); //se procesa la llegada
        } else {
            exit_user(evento_temp->usuario, lambda,sim_time);         //se procesa la salida
        }
        free(evento_temp);                  //se elimina el evento procesado

    }

    print_results(max_hops);// se imprimen los datos para jupyter
    return 0;

}

