#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <bits/stdc++.h>

using namespace std;
int *memg;
int *litnum ;
int *litstr ;
int *datanum ;
char *datastr ;
int *workload;


void writeMemg(string nameFile,int limit, int *memg,int *litnum,int limiteNum,int *litstr,int limiteStr){
  ifstream archivo(nameFile);
  char linea[128];
  int base,tamano,limite,cont = 0,cont2=0,cont3=0;
  if(archivo.fail())cerr << "Error al abrir el archivo Pruebas.txt" << endl;
  else
    while(!archivo.eof()){
      archivo.getline(linea, sizeof(linea));
      //cout<<"Limite: "<< (limiteNum>>2)<<" limite str: "<<(limiteStr>>2)<<endl;
      if (string(linea)!="" && cont<15){
        string value = string(linea);
        base = stoi(value.substr(0,4),0,16);
        tamano = stoi( value.substr(4,4),0,16);
        unsigned int valor = (base<<16)|tamano;
        *(memg + cont) = valor;
      }else if(string(linea)!="" && cont<(limiteNum>>2)){
        //cout <<"Entre a litnum"<<endl;
        string value = string(linea);
        base = stoi(value.substr(0,4),0,16);
        tamano = stoi( value.substr(4,4),0,16);
        unsigned int valor = (base<<16)|tamano;
        *(litnum+cont2 ) = valor;
        cont2++;
      }else if(string(linea)!="" && cont<(limiteStr>>2)){
        //cout <<"Entre a litstr"<<endl;
        string value = string(linea);
        base = stoi(value.substr(0,4),0,16);
        tamano = stoi( value.substr(4,4),0,16);
        unsigned int valor = (base<<16)|tamano;
        *(litstr + cont3) = valor;
        cont3++;
      }
        cont++;
    }
    archivo.close();
}

int readMemg(int *memg, int pos){
  return *(memg+pos);
}

int readLitnum(int *litnum, int pos){
  return *(litnum+pos);
}

int readLitstr(int *litstr, int pos){
  return *(litstr+pos);
}

char* readDatastr(char *datastr, int pos, int tamano){
  char* cadena[tamano];
  for(int i=0;i<tamano;i++){
    cadena[i]=(char *)(datastr+pos+i);
  }
  return *cadena;
}

int readDatanum(int *datanum, int pos){
  return *(datanum+pos);
}

void writeDatastr(char *datastr, int pos, char* data){
  for(int i=0; i<strlen(data); i++){
    *(datastr + pos + i) = (char)data[i];
  }
}

void writeDatanum(int *datanum, int pos, int data){
  *(datanum + pos) = data;
}

char* createMemory(int max, char* name){
  int shm = shm_open(name, O_CREAT | O_RDWR | O_EXCL, 0600);
  off_t size_mem = max;
  if (shm == -1) {
    cerr << "Shared memory already created" << endl;
    shm = shm_open(name, O_RDWR , 0600);
    char *pMem = static_cast<char*>(mmap(NULL, size_mem, PROT_READ | PROT_WRITE,
                                        MAP_SHARED, shm, 0));
    return pMem;
  }

  if (ftruncate(shm, size_mem) == -1) {
    cerr << "Problems with memory size" << endl;
    return (char*)1;
  }

  char *pMem = static_cast<char*>(mmap(NULL, size_mem, PROT_READ | PROT_WRITE,
                                       MAP_SHARED, shm, 0));

  if ((void *) pMem == (void *) -1) {
    cerr << "Problems with memory map" << endl;
    return (char*)1;
  }

  return pMem;
}


int mainMemory(int argc, char* argv[]) {
  if (argc != 3) {
    cerr << "Usage control: <shmname>" << endl;
    return 1;
  }
  ifstream archivo(argv[2]);
  char linea[128];
  int base[6],tamano[6],limite[6],max = 0,cont = 0;
  if(archivo.fail()) cerr << "Error al abrir el archivo Pruebas.txt" << endl;
  else
    while(!archivo.eof()){
        archivo.getline(linea, sizeof(linea));
        if (string(linea)!=""){
          string value = string(linea);
          if(cont<6){
            tamano[cont] = stoi(value.substr(4,4),0,16);
            base[cont] = (stoi(value.substr(0,4),0,16));
            if(cont==2 || cont==4){
              limite[cont]=(base[cont]<<2)+tamano[cont]+(4-(tamano[cont]%4));
            }else{
              limite[cont]=(base[cont]<<2)+(tamano[cont]<<2);
            }
          }
          //cout << base[cont] <<" "<< tamano[cont] << " " << limite[cont] << endl;
          cont++;
        }

    }
  max = limite[5];
  archivo.close();

  char *pMem = createMemory(max,argv[1]);

  memg = (int *)pMem;
  litnum = (int *)pMem+base[1];
  litstr = (int *)pMem+base[2];
  datanum = (int *)pMem+base[3];
  datastr = (char*)((int *)pMem+base[4]);
  workload = (int *)pMem+base[5];

  writeMemg(string(argv[2]),tamano[0],memg,litnum,limite[1],litstr,limite[2]);

  /*for(int i = 0; i<15;i++){
    unsigned int  var=*(memg+i);
    cout << var<<endl;
  }*/

  //char valor[32768]={'h','o','l','a',' ','m','u','n','d','o',' ','m','k'};
  //cout << "Salida: "<< valor << endl;
  //writeDatastr(datastr,0,valor);
  //writeDatanum(datanum,0,13);
  //cout << readDatanum(datanum,0)<<endl;
  //cout << readDatastr(datastr,0,strlen(valor))<<endl;
  //int result = readMemg(memg, 2);
  //cout << "result: " << result << endl;*/
  return 0;
}

int main(int argc, char* argv[]){
    mainMemory(argc,argv);
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
              cout << "Es: "<< readLitnum(litnum,poslitnum) <<endl;


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
