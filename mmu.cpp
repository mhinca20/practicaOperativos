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
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;
int *memg;
int *litnum ;
int *litstr ;
int *datanum ;
char *datastr ;
sem_t *workload;


void writeMemg(string nameFile,int limit, int *memg,int *litnum,int limiteNum,int *litstr,int limiteStr){
  ifstream archivo(nameFile);
  char linea[128];
  int base,tamano,limite,cont = 0,cont2=0,cont3=0;
  if(archivo.fail())cerr << "Error al abrir el archivo para escribir en el segmento" << endl;
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


int mainMemory(char* memoryName, string fileName) {
  ifstream archivo(fileName);
  char linea[128];
  int base[6],tamano[6],limite[6],max = 0,cont = 0;
  if(archivo.fail()) cerr << "Error al abrir el archivo de configuracion" << endl;
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

  char *pMem = createMemory(max,memoryName);
  memg = (int *)pMem;
  litnum = (int *)pMem+base[1];
  litstr = (int *)pMem+base[2];
  datanum = (int *)pMem+base[3];
  datastr = (char*)((int *)pMem+base[4]);
  workload = (sem_t*)((int *)pMem+base[5]);

  // ejemplo semaforos
  sem_t escr, mutex;
  sem_init(&mutex,0,1);
  sem_init(&escr,0,1);
  *(workload) = escr;
  *(workload+1) = mutex;
  for(int i=0; i<100; i++){
    sem_wait(&(*(workload+1)));
    sem_post(&(*(workload+1)));
  }

  //cout <<"data num: "<< base[4]<<endl;
  writeMemg(string(fileName),tamano[0],memg,litnum,limite[1],litstr,limite[2]);

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
/*
//prioridad en lectores
void escritor(){
  sem_wait(&escr);
  // se realiza la escritura
  sem_post(&escr);
}

void lector(){
  sem_wait(&mutex);
  cuentalect := cuentalect + 1;
  if cuentalect = 1
  then espera (escr );
  senal (mutex );
  .
  .
  .
  se realiza la lectura
  .
  .
  .
  espera (mutex );
  cuentalect := cuentalect - 1;
  if cuentalect = 0
  then senal (escr );
  senal (mutex );
}

//prioridad en escritores
void escritor(){
  espera (mutexcescr );
  cuentaescr := cuentaescr + 1;
  if cuentaescr = 1
  then espera (slect );
  senal (mutexcescr );
  espera (sescr );
  ESCRIBIRDATOS();
  senal (sescr );
  espera (mutexcescr );
  cuentaescr := cuentaescr - 1;
  if cuentaescr = 0
  then senal (slect );
  senal (mutexcescr );
}

void lector(){
  espera (mutex );
  espera (slect );
  espera (mutexlect );
  cuentalect := cuentalect + 1;
  if cuentalect = 1
  then espera (sescr );
  senal (mutexlect );
  senal (slect );
  senal (mutex );
  LEERDATOS();
  espera (mutextlect );
  cuentalect := cuentalect - 1;
  if cuentalect = 0
  then senal (sescr );
  senal (mutexlect );
}
*/

int interprete(int opcode, string value,int pc){
  unsigned int memref;
  unsigned int memref1;
  unsigned int memref2;
  unsigned int poslitnum,posdatanum;
  unsigned int poslitstr,posdatastr;
  unsigned int trans;
  unsigned int op,enteroDes;

  int politica = *(memg+6);
  int inicio = politica>>16;
  int fin = politica&65535;
  int nroLineas = fin-inicio;

  int posPolitica = *(memg+inicio);
  int segmento=posPolitica>>31;
  int num=(posPolitica>>16)&32767;
  int num1=posPolitica&32767;

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
      cout << "datanum: " << datanum+memref << " - " << datanum << endl;
      // Falta bandera de si tiene politica o no.
      if((datanum+memref)>=datanum+num || (datanum+memref)<=datanum+num1){
        writeDatanum(datanum, memref, readValue);
      }else{
        writeDatanum(datanum, memref, readValue);
      }

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
      //cout<<"salida de readvalue: "<<readValue<<endl;
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
      if (trans){
        if(op==0){
          int readValue1 = readDatanum(datanum,memref1);
          int readValue2 = readDatanum(datanum,memref2);
          int result= readValue1+readValue2;
          writeDatanum(datanum,memref,result);
        }else if(op==1){
          int readValue1 = readDatanum(datanum,memref1);
          int readValue2 = readDatanum(datanum,memref2);
          int result= readValue1-readValue2;
          writeDatanum(datanum,memref,result);

        }else if(op==2){
          int readValue1 = readDatanum(datanum,memref1);
          int readValue2 = readDatanum(datanum,memref2);
          int result= readValue1*readValue2;
          writeDatanum(datanum,memref,result);

        }else if(op==3){
          int readValue1 = readDatanum(datanum,memref1);
          int readValue2 = readDatanum(datanum,memref2);
          int result= readValue1/readValue2;
          writeDatanum(datanum,memref,result);

        }else if(op==4){
          int readValue1 = readDatanum(datanum,memref1);
          int readValue2 = readDatanum(datanum,memref2);
          int result= readValue1%readValue2;
          writeDatanum(datanum,memref,result);

        }

      }else{
        if (op==0){
          char* readValue1 = readDatastr(datastr,memref1,128);
          char* readValue2 = readDatastr(datastr,memref2,128);
          string cadena= (string)(readValue1);
          string cadena1= (string)(readValue2);
          string mayor;
          string result;
          int longitud;
          if (cadena.size()<cadena1.size()){
            longitud=cadena.size();
            mayor=cadena1;
          }else{
            longitud=cadena1.size();
            mayor=cadena;
          }
          for (int i=0; i<longitud;i++){
            int a;
            a= (cadena[i]+cadena1[i]);
            a= a%127;
            result+= a;
          }
          result.append(mayor,longitud,mayor.size()-longitud);
          char* result2= (char*)result.c_str();
          writeDatastr(datastr,memref,result2);
        }else if(op==1){
          char* readValue1 = readDatastr(datastr,memref1,128);
          char* readValue2 = readDatastr(datastr,memref2,128);
          string cadena= (string)(readValue1);
          string cadena1= (string)(readValue2);
          string mayor;
          string result;
          int longitud;
          if (cadena.size()<cadena1.size()){
            longitud=cadena.size();
            mayor=cadena1;
          }else{
            longitud=cadena1.size();
            mayor=cadena;
          }
          for (int i=0; i<longitud;i++){
            int a;
            a= (cadena[i]-cadena1[i]);
            a= a%127;
            if (a<0){
              a= a*(-1);
            }
            result+= a;
          }
          result.append(mayor,longitud,mayor.size()-longitud);
          char* result2= (char*)result.c_str();
          writeDatastr(datastr,memref,result2);

        }else if(op==2){
          char* readValue1 = readDatastr(datastr,memref1,128);
          char* readValue2 = readDatastr(datastr,memref2,128);
          string cadena= (string)(readValue1);
          string cadena1= (string)(readValue2);
          string mayor;
          string result;
          int longitud;
          if (cadena.size()<cadena1.size()){
            longitud=cadena.size();
            mayor=cadena1;
          }else{
            longitud=cadena1.size();
            mayor=cadena;
          }
          for (int i=0; i<longitud;i++){
            int a;
            a= (cadena[i]*cadena1[i]);
            a= a%127;
            result+= a;
          }
          result.append(mayor,longitud,mayor.size()-longitud);
          char* result2= (char*)result.c_str();
          writeDatastr(datastr,memref,result2);

        }else if(op==3){
          char* readValue1 = readDatastr(datastr,memref1,128);
          char* readValue2 = readDatastr(datastr,memref2,128);
          string cadena= (string)(readValue1);
          string cadena1= (string)(readValue2);
          string mayor;
          string result;
          int longitud;
          if (cadena.size()<cadena1.size()){
            longitud=cadena.size();
            mayor=cadena1;
          }else{
            longitud=cadena1.size();
            mayor=cadena;
          }
          for (int i=0; i<longitud;i++){
            int a;
            a= (cadena[i]/cadena1[i]);
            a= a%127;
            result+= a;
          }
          result.append(mayor,longitud,mayor.size()-longitud);
          char* result2= (char*)result.c_str();
          writeDatastr(datastr,memref,result2);

        }else if(op==4){
          char* readValue1 = readDatastr(datastr,memref1,128);
          char* readValue2 = readDatastr(datastr,memref2,128);
          string cadena= (string)(readValue1);
          string cadena1= (string)(readValue2);
          string mayor;
          string result;
          int longitud;
          if (cadena.size()<cadena1.size()){
            longitud=cadena.size();
            mayor=cadena1;
          }else{
            longitud=cadena1.size();
            mayor=cadena;
          }
          for (int i=0; i<longitud;i++){
            int a;
            a= (cadena[i]%cadena1[i]);
            a= a%127;
            result+= a;
          }
          result.append(mayor,longitud,mayor.size()-longitud);
          char* result2= (char*)result.c_str();
          writeDatastr(datastr,memref,result2);


        }
      }
      return pc;
    }

    case 6:{
      cout << "Value: "<< value<<endl;
      cout << "Opcode: "<<opcode<<endl;
      trans=((stoi(value.substr(1,1),0,16))&8)>>3;
      cout<<"transf: "<< trans<<endl;
      memref=(stoi(value.substr(1,4),0,16))&32767;
      cout<<"memref: "<< memref<<endl;
      if (trans){
          posdatanum=(stoi(value.substr(5,4),0,16)>>1);
          cout <<"litnum: "<< posdatanum<<endl;
          enteroDes=(stoi(value.substr(8,5),0,16)&131068);

          int readValue=readLitnum(litnum,enteroDes);
          int readValue2= readDatanum(datanum,readValue+posdatanum);
          writeDatanum(datanum,memref,readValue2);
      }else{
          posdatastr=(stoi(value.substr(5,4),0,16)>>1);
          cout <<"litstr: "<< posdatastr<<endl;
          enteroDes=(stoi(value.substr(8,5),0,16)&131068);
          int readValue=readLitnum(litnum,enteroDes);
          char* readValue2 = readDatastr(datastr,posdatastr+readValue,128);
          writeDatastr(datastr,memref,readValue2);
      }
      return pc;
    }
    case 7:{
      cout << "Value: "<< value<<endl;
      cout << "Opcode: "<<opcode<<endl;
      trans=((stoi(value.substr(1,1),0,16))&8)>>3;
      cout<<"transf: "<< trans<<endl;
      memref=(stoi(value.substr(1,4),0,16))&32767;
      cout<<"memref: "<< memref<<endl;
      if (trans){
          enteroDes=(stoi(value.substr(5,4),0,16)>>1);
          posdatanum=(stoi(value.substr(8,5),0,16)&131068);
          cout <<"litnum: "<< posdatanum<<endl;
          int readValue=readLitnum(litnum,enteroDes);
          int readValue2=readDatanum(datanum,posdatanum);
          writeDatanum(datanum,memref+readValue,readValue2);
      }else{
          enteroDes=(stoi(value.substr(5,4),0,16)>>1);
          posdatastr=(stoi(value.substr(8,5),0,16)&131068);
          cout <<"litstr: "<< posdatastr<<endl;
          int readValue=readLitnum(litnum,enteroDes);
          char* readValue2 = readDatastr(datastr,posdatastr,128);
          writeDatastr(datastr,memref+readValue,readValue2);
      }
      return pc;
    }
    case 8:{
      cout << "Value: "<< value<<endl;
      cout << "Opcode: "<<opcode<<endl;
      memref=(stoi(value.substr(1,4),0,16))>>1;
      cout<<"memref: "<< memref<<endl;
      int readValue;
      cout<<"Ingrese un entero: ";
      cin>>readValue;
      writeDatanum(datanum,memref,readValue);
      return pc;
    }
    case 9:{
      cout << "Value: "<< value<<endl;
      cout << "Opcode: "<<opcode<<endl;
      memref=(stoi(value.substr(1,4),0,16))>>1;
      cout<<"memref: "<< memref<<endl;
      int readValue=readDatanum(datanum,memref);
      cout<<"writeInt: "<<readValue<<endl;
      return pc;

    }
    case 10:{
      cout << "Value: "<< value<<endl;
      cout << "Opcode: "<<opcode<<endl;
      memref=(stoi(value.substr(1,4),0,16))>>1;
      cout<<"memref: "<< memref<<endl;
      memref2=((stoi(value.substr(4,5),0,16))&131068)>>2;
      string valor;
      for (int i=0;i<memref2;i++){
        char a;
        cout<<"Valor: ";
        cin>>a;
        if (a=='\0'){
          i=memref2;
        }
        cout<<a<<endl;
        valor+=a;
      }
      char* valor2= (char* )valor.c_str();
      writeDatastr(datastr,memref, valor2);
      return pc;
    }
    case 11:{
      cout << "Value: "<< value<<endl;
      cout << "Opcode: "<<opcode<<endl;
      memref=(stoi(value.substr(1,4),0,16))>>1;
      cout<<"memref: "<< memref<<endl;
      char* readValue=readDatastr(datastr,memref,128);
      cout<<"writeStr: "<<readValue<<endl;
      return pc;
    }
    case 12:{
      cout << "Value: "<< value<<endl;
      cout << "Opcode: "<<opcode<<endl;
      memref=(stoi(value.substr(1,4),0,16))>>1;
      int readValue=readLitnum(litnum,memref);
      pc=readValue;
      return pc;
    }
    case 13:{
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

      if (trans){
        if (op==0){//>=
          int readValue=readDatanum(datanum,memref);
          int readValue2=readDatanum(datanum,memref1);
          if(readValue>=readValue2){
            pc=readLitnum(litnum,memref2);
          }
        }else if(op==1){
          int readValue=readDatanum(datanum,memref);
          int readValue2=readDatanum(datanum,memref1);
          if(readValue>readValue2){
            pc=readLitnum(litnum,memref2);
          }
        }else if(op==2){
          int readValue=readDatanum(datanum,memref);
          int readValue2=readDatanum(datanum,memref1);
          if(readValue<=readValue2){
            pc=readLitnum(litnum,memref2);
          }
        }else if (op==3){
          int readValue=readDatanum(datanum,memref);
          int readValue2=readDatanum(datanum,memref1);
          if(readValue<readValue2){
            pc=readLitnum(litnum,memref2);
          }
        }else if(op==4){
          int readValue=readDatanum(datanum,memref);
          int readValue2=readDatanum(datanum,memref1);
          if(readValue==readValue2){
            pc=readLitnum(litnum,memref2);
          }
        }else if(op==5){
          int readValue=readDatanum(datanum,memref);
          int readValue2=readDatanum(datanum,memref1);
          if(readValue!=readValue2){
            pc=readLitnum(litnum,memref2);
          }
        }
      }else{//char* readDatastr(char *datastr, int pos, int tamano)
        if (op==0){//>=
          char* readValue=readDatastr(datastr,memref,1);
          char* readValue2=readDatastr(datastr,memref1,1);
          if(readValue[0]>=readValue2[0]){
            pc=readLitnum(litnum,memref2);
          }
        }else if (op==1){
          char* readValue=readDatastr(datastr,memref,1);
          char* readValue2=readDatastr(datastr,memref1,1);
          if(readValue[0]>readValue2[0]){
            pc=readLitnum(litnum,memref2);
          }
        }else if(op==2){
          char* readValue=readDatastr(datastr,memref,1);
          char* readValue2=readDatastr(datastr,memref1,1);
          if(readValue[0]<=readValue2[0]){
            pc=readLitnum(litnum,memref2);
          }
        }else if (op==3){
          char* readValue=readDatastr(datastr,memref,1);
          char* readValue2=readDatastr(datastr,memref1,1);
          if(readValue[0]<readValue2[0]){
            pc=readLitnum(litnum,memref2);
          }
        }else if (op==4){
          char* readValue=readDatastr(datastr,memref,1);
          char* readValue2=readDatastr(datastr,memref1,1);
          if(readValue[0]==readValue2[0]){
            pc=readLitnum(litnum,memref2);
          }
        }else if (op==5){
          char* readValue=readDatastr(datastr,memref,1);
          char* readValue2=readDatastr(datastr,memref1,1);
          if(readValue[0]!=readValue2[0]){
            pc=readLitnum(litnum,memref2);
          }
        }
      }
      return pc;
    }
    case 14:{
      cout << "Value: "<< value<<endl;
      cout << "Opcode: "<<opcode<<endl;
      pc=-1;
      return pc;
    }
    case 15:{
      cout << "Value: "<< value<<endl;
      cout << "Opcode: "<<opcode<<endl;
      pc=-2;
      return pc;
    }
    return pc;
  }

}

void proceso(string file){
  ifstream archivo(file);
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
        if (i==-1){
          cout <<"Pare por halt"<<endl;
          i=instructions.size();
        }
        if (i==-2){
          cout <<"Pare por break"<<endl;
          i=instructions.size();
        }

      }
  }
}

int main(int argc, char* argv[]){
  char* memoryName;
  string fileName;
  string archivo[argc];

  if (argc>1) {
    if (string(argv[1]).compare("-n")==0 && argc==4){
      memoryName=argv[2];
      fileName=argv[3];

    }
    if(argc == 2){
      fileName=argv[1];
      cout<<"Ingrese el nombre de la memoria: ";
      string a;
      cin>>a;
      memoryName=(char* )a.c_str();
    }
    if(argc == 3){
      memoryName=argv[2];
      cout<<"Ingrese el nombre del archivo de configuracion de memoria: ";
      cin>>fileName;
    }
  }else{
    cout<<"Ingrese el nombre de la memoria: ";
    string a;
    cin>>a;
    memoryName=(char* )a.c_str();
    cout<<"Ingrese el nombre del archivo de configuracion de memoria: ";
    cin>> fileName;
  }
  int numArch=0;
  cout << "ingrese numero de archivos: ";
  cin>>numArch;
  string archivos[numArch];
  for(int i=0; i<numArch; i++){
    cout<<"ingrese nombre del archivo: ";
    cin>>archivos[i];
  }
  mainMemory(memoryName,fileName);

  pid_t proc;
  for(int i=0; i<numArch; i++){
    if ((proc = ::fork()) == 0) {
      cout << "---------------------------proceso " << i << endl;
      proceso(archivos[i]);
      _exit(EXIT_SUCCESS);
    }
  }
  int status;
  waitpid(proc, &status, 0);
  //waitpid(proc2, &status, 0);

}
