#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <bits/stdc++.h>
#include <map>

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
      }else if(string(linea)!="" && cont<(limiteNum)){
        //cout <<"Entre a litnum"<<endl;
        string value = string(linea);
        base = stoi(value.substr(0,4),0,16);
        tamano = stoi( value.substr(4,4),0,16);
        unsigned int valor = (base<<16)|tamano;
        *(litnum+cont2 ) = valor;
        cont2++;
      }else if(string(linea)!="" && cont<(limiteStr)){
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

char* readLitstr(int *litstr, int pos, int limite){
  char* value[128];
  for(int i = pos; *((char*)litstr+i)!='\0' && i < limite; i++){
    value[i] = (char*)((char*)litstr+i);
  }
  return *value; 
}

char* readDatastr(char *datastr, int pos, int tamano){
  char* cadena[128];
  for(int i=0;*((char*)datastr+i)!='\0' && i<tamano;i++){
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
  *(datastr + pos + strlen(data)) = '\0';
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
              limite[cont]=(base[cont])+tamano[cont]+(4-(tamano[cont]%4));
            }else{
              limite[cont]=(base[cont])+(tamano[cont]);
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
  cout <<"data num: "<< base[4]<<endl;
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

int interprete(int opcode, string value,int pc){
  unsigned int memref;
  unsigned int poslitnum,posdatanum;
  unsigned int poslitstr,posdatastr;
  unsigned int trans;
  unsigned int op;
  switch(opcode) {
    case 0:{
      
      cout << "Value: "<< value<<endl;
      cout << "Opcode: "<<opcode<<endl;
      memref=(stoi(value.substr(1,4),0,16))>>1;
      cout<<"memref: "<< memref<<endl;
      poslitnum=((stoi(value.substr(4,5),0,16))&131068)>>2;
      cout <<"litnum: "<< poslitnum<<endl;
      cout << "Es: "<< readLitnum(litnum,poslitnum) <<endl;
      int readValue = readLitnum(litnum,poslitnum);
      writeDatanum(datanum, memref, readValue);


      return pc;
    }
    case 1:{
      cout << "Value: "<< value<<endl;
      cout << "Opcode: "<<opcode<<endl;
      memref=(stoi(value.substr(1,4),0,16))>>1;
      cout<<"memref: "<< memref<<endl;
      poslitstr=(((stoi(value.substr(4,5),0,16))&131068)>>2);
      cout <<"litstr: "<< poslitstr<<endl;
      char* readValue = readLitstr(litstr,poslitstr,10);
      cout<<"salida de readvalue: "<<readValue<<endl;
      writeDatastr(datastr, memref, readValue);
      
      return pc;
    }
    case 2:{
      cout << "Value: "<< value<<endl;
      cout << "Opcode: "<<opcode<<endl;
      memref=(stoi(value.substr(1,4),0,16))>>1;
      cout<<"memref: "<< memref<<endl;
      poslitnum=((stoi(value.substr(4,5),0,16))&131068)>>2;
      cout <<"litnum: "<< poslitnum<<endl;
      int readValue = readLitnum(litnum,poslitnum);
      writeDatanum(datanum,memref,(readValue+pc));

      return pc; 
    }
    case 3:{
      cout << "Value: "<< value<<endl;
      cout << "Opcode: "<<opcode<<endl;
      memref=(stoi(value.substr(1,4),0,16))>>1;
      cout<<"memref: "<< memref<<endl;
      int readValue = readDatanum(datanum,memref);
      
      return (readValue-1);
    }
    case 4:{
      cout << "Value: "<< value<<endl;
      cout << "Opcode: "<<opcode<<endl;
      trans=((stoi(value.substr(1,1),0,16))&8)>>3;
      cout<<"transf: "<< trans<<endl;
      memref=(stoi(value.substr(1,4),0,16))&32767;
      cout<<"memref: "<< memref<<endl;
      if (trans){
          posdatanum=(stoi(value.substr(5,4),0,16)>>1);
          cout <<"litnum: "<< posdatanum<<endl;
          int readValue = readDatanum(datanum,posdatanum);
          writeDatanum(datanum,memref,readValue);
      }else{
          posdatastr=(stoi(value.substr(5,4),0,16)>>1);
          cout <<"litstr: "<< posdatastr<<endl;
          char* readValue = readDatastr(datastr,posdatastr,128);
          writeDatastr(datastr,memref,readValue);
      }
      return pc;
    }
    case 5:{

      unsigned int memref1;
      unsigned int memref2;
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

      return pc;
    }
    case 6:{
      cout << "Opcode: "<<opcode<<endl;
      return pc;
    }
    case 7:{
      cout << "Opcode: "<<opcode<<endl;
      return pc;
    }
    return pc;


  }

}

int main(int argc, char* argv[]){

    mainMemory(argc,argv);
    ifstream archivo("test.bew");
    char linea[10000];
    int opcode;
    
    string value;
    //mapa con id:pc y clave:linea --< 1:a:=4
    map<int,string> instructions;
    readLitstr(litstr,0,11);

    //
    if(archivo.fail()) cerr << "Error al abrir el archivo" << endl;
    else{

        archivo.getline(linea, sizeof(linea));
        int pc = 0;
        
        //El primer recorrido
        for (int i=0; i<strlen(linea);i++){

          opcode = stoi(string(linea).substr(i,1),0,16);

          //cout << "i: "<< i<<endl;
          switch(opcode) {
            case 0:{
              instructions[pc]=string(linea).substr(i,16);


              i+=15;
              break;
            }
            case 1:{
              instructions[pc]=string(linea).substr(i,16);
              i+=15;
              break;
            }
            case 2:{
              instructions[pc]=string(linea).substr(i,16);
              i+=15;
              break;
            }
            case 3:{
              instructions[pc]=string(linea).substr(i,8);
              i+=7;
              break;
            }
            case 4:{
              instructions[pc]=string(linea).substr(i,16);
              i+=15;
              break;
            }
            case 5:{

              instructions[pc]=string(linea).substr(i,16);
              i+=15;
              break;
            }
            case 6:{
              instructions[pc]=string(linea).substr(i,16);
              i+=15;
              break;
            }
            case 7:{
              instructions[pc]=string(linea).substr(i,16);
              i+=15;
              break;
            }
            case 8:{
              instructions[pc]=string(linea).substr(i,8);
              i+=7;
              break;
            }
            case 9:{
              instructions[pc]=string(linea).substr(i,8);
              i+=7;
              break;
            }
            case 10:{
              instructions[pc]=string(linea).substr(i,16);
              i+=15;
              break;
            }
            case 11:{
              instructions[pc]=string(linea).substr(i,8);
              i+=7;
              break;
            }
            case 12:{
              instructions[pc]=string(linea).substr(i,8);
              i+=7;
              break;
            }
            case 13:{
              instructions[pc]=string(linea).substr(i,16);
              i+=15;
              break;
            }
            case 14:{
              instructions[pc]=string(linea).substr(i,2);
              i++;
              break;
            }
            case 15:{
              instructions[pc]=string(linea).substr(i,2);
              i++;
              break;
            }
          }
          pc++;


        }
        for(int i = 0; i < instructions.size() ; i++)
        {
          opcode = stoi(instructions[i].substr(0,1),0,16);
          i = interprete(opcode, instructions[i],i);
        }
    }
    return 0;
}
