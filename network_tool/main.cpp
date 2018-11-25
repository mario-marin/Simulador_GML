#include <iostream>
#include <math.h>
#include <random>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>


using namespace std;

int main(int argc, char *argv[])
{
    cout << "Testing!" << endl;
    string file = argv[1];
    string rut = file+".rut";

    ifstream infile("./Redes\ y\ Rutas/Rutas/"+rut);
    string line;
    string temp_str;
    int counter=0;
    int temp;
    vector<int> links;  //links         [links_id]->[contador]
    vector<vector<int>> users;  //usuarios      [usuario_id]->[llegadas,blokeo,#hops,the_hops]

    int user_counter = 0;
    int node_1;
    int node_2;
    int load_status = 0;
    int hops = 0;
    int max_hops = 0;
    int hop_counter = 0;
    int init_flag=0;
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

    for (int i = 0; i < links.capacity(); ++i) {
        links[i]=0;
    }

    for (int i = 0; i < users.capacity(); ++i) {
        for (int ii = 0; ii < users[i][2]; ++ii) {   // se revisa si los enlases ocupados por el usuario se encuentran disponibles
                links[users[i][3+ii]] = links[users[i][3+ii]] + 1;
            }
    }

    int max = 0;

    for (int i = 0; i < links.capacity(); ++i) { // busca el enlase con mas capacidad requerida
        cout << links[i] << endl;
        if (links[i]>max) {
            max = links[i];
        }
    }

    cout << "max: " << max << endl;
    return 0;
}
