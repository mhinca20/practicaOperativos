#include <bits/stdc++.h>
#include <iostream>   // std::cout
#include <string>     // std::string, std::stoi
#include <confMemory.cpp>
using namespace std;

int main(){

    ifstream archivo("test.bew");
    char linea[10000];
    int opcode;
    unsigned int memref;
    unsigned int poslitnum;
    unsigned int poslitstr;
    unsigned int trans;
    unsigned int op;
    string value;
    if(archivo.fail()) cerr << "Error al abrir el archivo" << endl;
    else{

        archivo.getline(linea, sizeof(linea));
        for (int i=0; i<strlen(linea);i++){
          opcode = stoi(string(linea).substr(i,1),0,16);
          //cout << "i: "<< i<<endl;
          switch(opcode) {
            case 0:{
              value=string(linea).substr(i,16);
              cout << "Value: "<< value<<endl;
              cout << "Opcode: "<<opcode<<endl;
              memref=(stoi(value.substr(1,4),0,16))>>1;
              cout<<"memref: "<< memref<<endl;
              poslitnum=((stoi(value.substr(4,5),0,16))&131068)>>2;
              cout <<"litnum: "<< poslitnum<<endl;
              i+=15;
              break;
            }
            case 1:{
              value=string(linea).substr(i,16);
              cout << "Value: "<< value<<endl;
              cout << "Opcode: "<<opcode<<endl;
              memref=(stoi(value.substr(1,4),0,16))>>1;
              cout<<"memref: "<< memref<<endl;
              poslitstr=(((stoi(value.substr(4,5),0,16))&131068)>>2);
              cout <<"litstr: "<< poslitstr<<endl;
              i+=15;
              break;
            }
            case 2:{
              value=string(linea).substr(i,16);
              cout << "Value: "<< value<<endl;
              cout << "Opcode: "<<opcode<<endl;
              memref=(stoi(value.substr(1,4),0,16))>>1;
              cout<<"memref: "<< memref<<endl;
              poslitnum=((stoi(value.substr(4,5),0,16))&131068)>>2;
              cout <<"litnum: "<< poslitnum<<endl;
              i+=15;
              break;
            }
            case 3:{
              value=string(linea).substr(i,8);
              cout << "Value: "<< value<<endl;
              cout << "Opcode: "<<opcode<<endl;
              memref=(stoi(value.substr(1,4),0,16))>>1;
              cout<<"memref: "<< memref<<endl;
              i+=7;
              break;
            }
            case 4:{
              value=string(linea).substr(i,16);
              cout << "Value: "<< value<<endl;
              cout << "Opcode: "<<opcode<<endl;
              trans=(stoi(value.substr(1,1),0,16))&8;
              cout<<"transf: "<< trans<<endl;
              memref=(stoi(value.substr(2,4),0,16))&32767;
              cout<<"memref: "<< memref<<endl;
              if (trans){
                  poslitnum=(stoi(value.substr(5,4),0,16)>>1);
                  cout <<"litnum: "<< poslitnum<<endl;
              }else{
                  poslitstr=(stoi(value.substr(5,4),0,16)>>1);
                  cout <<"litstr: "<< poslitstr<<endl;
              }
              i+=15;
              break;
            }
            case 5:{

              unsigned int memref1;
              unsigned int memref2;
              value=string(linea).substr(i,16);
              cout << "Value: "<< value<<endl;
              cout << "Opcode: "<<opcode<<endl;
              op=(stoi(value.substr(1,1),0,16))>>1;
              cout<<"operation: "<< op<<endl;
              trans=(stoi(value.substr(1,1),0,16))&1;
              cout<<"transf: "<< trans<<endl;
              memref=(stoi(value.substr(2,4),0,16))>>1;
              cout<<"memref: "<< memref<<endl;
              memref1=((stoi(value.substr(5,5),0,16))&131068)>>2;
              memref2=((stoi(value.substr(9,5),0,16))&262136)>>3;
              cout<<"memref1: "<< memref1<<endl;
              cout<<"memref2: "<< memref2<<endl;

              i+=15;
              break;
            }
            case 6:{
              cout << "Opcode: "<<opcode<<endl;
              i+=15;
              break;
            }
            case 7:{
              cout << "Opcode: "<<opcode<<endl;
              i+=7;
              break;
            }

          }

        }

    }
    return 0;
}

        /*    string value = string(linea);
            opcode = stoi(value.substr(pos,1),0,16);
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
                pos+=64;
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

*/
