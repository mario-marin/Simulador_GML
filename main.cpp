#include <iostream>
#include <math.h>
#include <random>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "lcgrand.c"
#include "scheduler.c"
#include "general_utilities.cpp"

using namespace std;

//////////////////////--global variables--//////////////////////////////////////////
int generate;               //generate flag
float usage;                //numero de clientes en el canal
double blocked = 0;             //numero total de blockeos
double total_arrivals = 0;  //numero total de arrivos
vector<vector<int>> users;  //usuarios      [usuario_id]->[llegadas,blokeo,#hops,the_hops(links_id's)]
vector<vector<int>> wavelenght_map; //tabla de uso de los labdas de la red [lambda_id] -> [links_id]
vector<string> name_user;

random_device rd;
mt19937_64 seed (rd());

//////////////////////--funtions--//////////////////////////////////////////////////

int find_lambda(int user_id){//metodo First-fit
	int max_wavelenght = wavelenght_map.size();
	int lambda = -1;
	int link_id;
	bool stop;

	for (int i = 0; i < max_wavelenght; ++i)
	{
		stop = false;
		for (int x = 0; x < users[user_id][2]; ++x)
		{
			link_id = users[user_id][3+x];
			if (wavelenght_map[i][link_id] == -1) //longitud disponible en enlase
			{
				stop = false;
				continue;
			}else{
				stop = true;
				break;//longitud no disponible, ocupar la siguiente
			}
		}

		if (stop == false)
		{
			lambda = i;
			break;
		}
	}

	return lambda;
}

void reserve_lambda(int user_id,int lambda_id){//metodo First-fit
	int link_id;

	if (lambda_id != -1)
	{
		for (int x = 0; x < users[user_id][2]; ++x)
		{
			link_id = users[user_id][3+x];
			wavelenght_map[lambda_id][link_id] = user_id;
		}
	}else{
		cout << "Cant assign -1 lambda" <<endl;
	}
	
}
void free_lambda(int user_id,int lambda_id){//metodo First-fit
	int link_id;
	if (lambda_id != -1)
	{
		for (int x = 0; x < users[user_id][2]; ++x)
		{
			link_id = users[user_id][3+x];
			wavelenght_map[lambda_id][link_id] = -1;
		}
	}else{
		cout << "Cant free -1 lambda" <<endl;
	}
}

//------------

float probability(float lambda){ //calculo de tiempo exponencial
    return (-1.0/lambda)*log(1-lcgrand(seed()%100)); // probabilidad exponecial
}

int arrive_user(int id_user, float lambda,float mu, float sim_time){
    float prob_temp;
    total_arrivals++;        // se aumenta las llegadas totales
    bool enable_flag = true; // flag indica si todos los enlases del usuario estan disponibles

    int temp_lambda; 		 // lambda asignado para el usuario

    users[id_user][0] = users[id_user][0] + 1;      // se aumenta las llegadas del usuario entrante

    temp_lambda = find_lambda(id_user);

    if (temp_lambda != -1) //si se asgino un lambda valido
    {
    	reserve_lambda(id_user,temp_lambda);		  // se reservar los recursos
        if(generate  == 1){
            prob_temp = probability(mu);              //calculo de tiempo de salida
            pushEvento(crearEvento(0,id_user,temp_lambda,prob_temp+sim_time)); //se agrega al scheduler
        }
        return 100;
    }else{
 		blocked++;                                // se aumenta los usuarios bloqueados totales
        users[id_user][1] = users[id_user][1] + 1;// se aumentan el contador de bloqueos del usuario
        if(generate  == 1){
            prob_temp = probability(lambda);          // calculo de tiempo de siguiente llegada
            pushEvento(crearEvento(1,id_user,-1,prob_temp+sim_time)); //se agrega al scheduler
        }
        return 404;
    }

}

int exit_user(int id_user,int assigned_lambda, float lambda, float sim_time){
     float prob_temp;
     /*
     for (int i = 0; i < users[id_user][2]; ++i) { //se libera el espacio ocupado por el usuario en todos los enlases
         links[users[id_user][3+i]][1] = links[users[id_user][3+i]][1] - 1;
     }
     */
     free_lambda(id_user,assigned_lambda);
     if(generate  == 1){
         prob_temp = probability(lambda);                           //calculo de tiempo de llegada
         pushEvento(crearEvento(1,id_user,-1,prob_temp+sim_time)); //se agrega al scheduler
    }
     return 0;
}

void load_scheduler(){
    string line;
    string temp_str;
    ifstream time_line_in;
    time_line_in.open ("./timeline");
    int counter = 0;

    int tipo;
    int usuario;
    int lambda;
    double tiempo;

    while(getline(time_line_in,line)){
        istringstream stream(line);
        while(getline(stream,temp_str,',')){
            if (counter == 0){
                tipo = stoi(temp_str);
                counter++;
            }else if (counter == 1){
                usuario = stoi(temp_str);
                counter++;
            }else if (counter == 2){
                lambda = stoi(temp_str);
                counter++;
            }else if (counter == 3){
                tiempo = stod(temp_str);
                counter++;
            }
        }
        pushEvento(crearEvento(tipo,usuario,lambda,tiempo));
        counter = 0;
    }
}


////////////////////////////--code--///////////////////////////////////////


int main(int argc, char *argv[])  // argumentos : nombre_de_red, capcidad de enlases, nomero_de_llegadas_de_parada, t_on, carga_promedio
{
    string file = argv[1];
    string rut = file+".rut";
    int C = atoi(argv[2]);
    int max_hops = 0;
    float ton = atof(argv[3]);
    float carga = atof(argv[4]);
    float toff = (ton-carga*ton)/carga;
    float lambda = 1.0/(ton+toff);
    float mu = 1.0/(ton);
    float prob_temp;
    double sim_time = 0;
    generate = atoi(argv[5]);
    Evento * evento_temp;
  
    vector<Evento * > event_log;
    
    //-------------------routes loading---------------------
    
    load_routes_file(users,wavelenght_map,max_hops,rut,C,name_user);

    //-----------------init block--------------------------

    if (generate == 1){
        for (unsigned int i = 0; i < users.capacity(); ++i) {// se unizialisa la simulacion
            prob_temp = probability(lambda);
            pushEvento(crearEvento(1,i,-1,prob_temp));          //1 is on
        }  
    }
    
    //----------testing-----------------
    //load_scheduler();

    //-----------------simulation--------------------------

    double relative_error = 0.05;
    double z_alfa_2 = 1.96;
    double prob = 0;
    double ER = 100;
    double IC = 10000000000000000;

    ofstream time_line_out;

    if (generate == 1){
        time_line_out.open ("./timeline");
    }else if (generate == 0){
        load_scheduler();
    }

    do {     //simulation con condicion de parada

        evento_temp = popEvento();

        if (generate == 1){
            time_line_out << evento_temp->tipo << "," << evento_temp->usuario << "," << evento_temp->lambda << ","<< evento_temp->tiempo << endl;
        }
        

        if(sim_time != evento_temp->tiempo){//se verifica que el evento extraido no ocurrio al mismo tiempo que el evento anterior (coliciones)
            sim_time = evento_temp->tiempo;
        }

        if(evento_temp->tipo == 1){         //se verifica si es una llegada o salida
            arrive_user(evento_temp->usuario,lambda,mu,sim_time); //se procesa la llegada
        } else {
            exit_user(evento_temp->usuario,evento_temp->lambda , lambda,sim_time);         //se procesa la salida
        }
        free(evento_temp);                  //se elimina el evento procesado

        if(total_arrivals > 10000){
            prob = blocked/total_arrivals;
            //cout << prob << endl;
            if( prob != 0.0 && total_arrivals < 10000000){
                IC = prob + z_alfa_2* sqrt((prob*(1-prob))/total_arrivals);
                ER = prob + prob*relative_error/2;
            }else if(prob == 0.0 && total_arrivals > 10000000){
                break;
            }
        }

    }while(IC > ER);


    print_results(users,name_user,blocked,total_arrivals);// se imprimen los datos para jupyter
    save_wavelenght_map_csv(wavelenght_map);

    time_line_out.close();
    return 0;

}

