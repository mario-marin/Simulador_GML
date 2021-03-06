#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

void save_wavelenght_map_csv( vector<vector<int>> wavelenght_map,string file_name = "default_csv"){
	int max_wavelenght = wavelenght_map.size();
	int max_link_id = wavelenght_map[0].size();
	string value;

	ofstream csv;
    csv.open (file_name);
    csv << "Links id's:" << ",";
    for (int i = 0; i < max_link_id; i++)
    {
    	if(i == max_link_id-1){
    		csv << i << "\n";
    	}else{
    		csv << i << ",";
    	}
    }
    for (int x = 0; x < max_wavelenght; x++)
    {
    	csv << "Lambda_" << x << ",";
    	for (int i = 0; i < max_link_id; i++)
    	{

	    	if (wavelenght_map[x][i] == -1){
	    		value = "";
	    	}else{
	    		value = to_string(wavelenght_map[x][i]);
	    	}

	    	if(i == max_link_id-1){
	    		csv << value << "\n";
	    	}else{
	    		csv << value << ",";
	    	}
   		}
    }
    
    csv.close();  

}


void print_results(vector<vector<int>>users,vector<string> name_user,double blocked,double total_arrivals){//funcion que imprime datos para desplegar en jupyter
    cout << "Lista de usuarios con su probabilidad de bloqueo" << endl;
    for(unsigned int i=0;i < users.capacity();i++){
        if((float)users[i][0] == 0.0){
            cout << name_user[i] << " :" << "Nunca llego" << endl;
        }else{
            cout << name_user[i] << " :" << (float)users[i][1]/(float)users[i][0] << endl;
        }
        
    }
    cout << "Probabilidad de bloqueo de la red: " << blocked/total_arrivals << endl;             // se entrega el prob de blokeo de la red completa
}







void load_routes_file(vector<vector<int>> & users,vector<vector<int>> & wavelenght_map,int & max_hops, string rut,int C, vector<string> & name_user){

    ifstream infile("./Redes\ y\ Rutas/Rutas/"+rut);
    string line;
    string temp_str;
    int counter=0;
    int node_1;
    int node_2;
    int init_flag=0;
    int load_status = 0;
    int hops = 0;
    int hop_counter = 0;
    int user_counter = 0;
    int temp;

    while(getline(infile,line,'\t')){
        istringstream stream(line);
        while(getline(stream,temp_str,'\n')){
            if(temp_str=="\r"){
                continue;
            }
            if (counter==7) {         // se extrae la cantidad de usuarios totales
                temp = stoi(temp_str);
                users.resize(temp*(temp-1));
                name_user.resize(temp*(temp-1));
            }else if (counter == 11) {// se extrae la cantidad de enlases totales
                temp = stoi(temp_str);

                wavelenght_map.resize(C);
                for (int i = 0; i < C; ++i)
                {
                    wavelenght_map[i].resize(temp,-1);
                }

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
                            name_user[user_counter] = to_string(node_1) + "-" + to_string(node_2);
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

}
