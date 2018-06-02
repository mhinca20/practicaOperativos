#include <bits/stdc++.h>
using namespace std;

int main(){
    
    ifstream archivo("test.bew");
    char linea[128];
    int opcode;
    if(archivo.fail()) cerr << "Error al abrir el archivo Pruebas.txt" << endl;
    else
        while(!archivo.eof()){
            archivo.getline(linea, sizeof(linea));
            string value = string(linea);
            opcode = stoi(value.substr(2,1),0,16);
            cout<<"value: "<<value<<endl;
            cout <<"opcode: "<< opcode<< endl;
            unsigned int memref;
            unsigned int poslitnum;
            unsigned int poslitstr;
            unsigned int trans;
            unsigned int op;
            
            switch(opcode) {
            case 0:{    
                memref=(stoi(value.substr(3,4),0,16))>>1;
                cout<<"memref: "<< memref<<endl;
                poslitnum=((stoi(value.substr(6,5),0,16))&131068)>>2;
                cout <<"litnum: "<< poslitnum<<endl;
                break;
                }
                
            case 1:{    
                memref=(stoi(value.substr(3,4),0,16))>>1;
                cout<<"memref: "<< memref<<endl;
                
                poslitstr=(((stoi(value.substr(6,5),0,16))&131068)>>2);
                cout <<"litstr: "<< poslitstr<<endl;
                break;
                } 
            case 2:{    
                memref=(stoi(value.substr(3,4),0,16))>>1;
                cout<<"memref: "<< memref<<endl;
                poslitnum=((stoi(value.substr(6,5),0,16))&131068)>>2;
                cout <<"litnum: "<< poslitnum<<endl;
                break;
                }
            case 3:{
                memref=(stoi(value.substr(3,4),0,16))>>1;
                cout<<"memref: "<< memref<<endl;
               

                break;
            }

            case 4:{    
                trans=(stoi(value.substr(3,1),0,16))&8;
                cout<<"transf: "<< trans<<endl;
                memref=(stoi(value.substr(3,4),0,16))&32767;
                cout<<"memref: "<< memref<<endl;
                if (trans){
                    poslitnum=(stoi(value.substr(7,4),0,16)>>1);     
                    cout <<"litnum: "<< poslitnum<<endl;
                }else{
                    poslitstr=(stoi(value.substr(7,4),0,16)>>1);     
                    cout <<"litstr: "<< poslitstr<<endl;
                }
                break;
            }
            case 5:{
                unsigned int memref1;
                unsigned int memref2;
                op=(stoi(value.substr(3,1),0,16))>>1;
                cout<<"operation: "<< op<<endl;
                trans=(stoi(value.substr(3,1),0,16))&1;
                cout<<"transf: "<< trans<<endl;
                memref=(stoi(value.substr(4,4),0,16))>>1;
                cout<<"memref: "<< memref<<endl;
                memref1=((stoi(value.substr(7,5),0,16))&131068)>>2;
                memref2=((stoi(value.substr(11,5),0,16))&262136)>>3;
                cout<<"memref1: "<< memref1<<endl;
                cout<<"memref2: "<< memref2<<endl;
                
                break;
            }

                
            
            }

        }
    


    return 0;
}
