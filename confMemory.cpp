#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

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

  int *memg = (int *)pMem;
  int *litnum = (int *)pMem+base[1];
  int *litstr = (int *)pMem+base[2];
  int *datanum = (int *)pMem+base[3];
  char *datastr = (char*)((int *)pMem+base[4]);
  int *workload = (int *)pMem+base[5];

  writeMemg(string(argv[2]),tamano[0],memg,litnum,limite[1],litstr,limite[2]);

  for(int i = 0; i<15;i++){
    unsigned int  var=*(memg+i);
    cout << var<<endl;
  }

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
