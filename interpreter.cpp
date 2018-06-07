#include "interpreter.h"

int politicas[4];
int inicio[4];
int fin[4];
//int nroLineas[4];
int inicioLineas[8];
int finLineas[8];
int cuentaescr=0;
int cuentalect=0;
int cuentalect2=0;
int bloqueo=1;

Interpreter::Interpreter(int *memg1, int *litnum1, int *litstr1, int *datanum1, char *datastr1, sem_t *workload1):
 memg(memg1), litnum(litnum1), litstr(litstr1), datanum(datanum1), datastr(datastr1),workload(workload1){


 }

Interpreter::~Interpreter() {}

void Interpreter::writeMemg(string nameFile,int limit, int *memg,int *litnum,int limiteNum,int *litstr,int limiteStr){
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

int Interpreter::readMemg(int *memg, int pos){
  return *(memg+pos);
}

int Interpreter::readLitnum(int *litnum, int pos){
  return *(litnum+pos);
}

char* Interpreter::readLitstr(int *litstr, int pos, int limite){
  char* value[128];
  for(int i = pos; *((char*)litstr+i)!='\0' && i < limite; i++){
    value[i] = (char*)((char*)litstr+i);
  }
  return *value;
}

char* Interpreter::readDatastr(char *datastr, int pos, int tamano,int politica){
  char* cadena[128];

  if (politica==1){//prioridad lectores

    cout<<"Entre a la politica 1 de readDatastr"<<endl;
    sem_wait(&(*(workload+1)));
    cuentalect++;
    if (cuentalect == 1){
      sem_wait(&(*(workload)));
      sem_post(&(*(workload+1)));
      for(int i=0;*((char*)datastr+i)!='\0' && i<tamano;i++){
        cadena[i]=(char *)(datastr+pos+i);
      }
      sem_wait(&(*(workload+1)));
      cuentalect--;
    }
    if (cuentalect == 0){
      sem_post(&(*(workload)));
      sem_post(&(*(workload+1)));
    }
  }else if(politica==2){//prioridad escritores

    cout<<"Entre a la politica 2 de readDatastr"<<endl;
    sem_wait (&(*(workload+5)));
    sem_wait (&(*(workload+3)));
    sem_wait (&(*(workload+6)));
    cuentalect2++;
    if (cuentalect2 == 1){
      sem_wait (&(*(workload+4)));
      sem_post (&(*(workload+6)));
      sem_post (&(*(workload+3)));
      sem_post (&(*(workload+5)));
      for(int i=0;*((char*)datastr+i)!='\0' && i<tamano;i++){
        cadena[i]=(char *)(datastr+pos+i);
      }

      sem_wait (&(*(workload+6)));
      cuentalect2 --;
    }
    if (cuentalect2 == 0){
      sem_post (&(*(workload+4)));
      sem_post (&(*(workload+6)));
    }

  }else if(politica==3){
    sem_wait(&(*(workload+7)));
    bloqueo--;
    cout<<"Entre a la politica 3 readDatastr------bloque: "<<bloqueo<<endl;
    for(int i=0;*((char*)datastr+i)!='\0' && i<tamano;i++){
      cadena[i]=(char *)(datastr+pos+i);
    }
    bloqueo++;
    cout<<"Bloqueo: "<<bloqueo<<endl;
    sem_post(&(*(workload+7)));

  }else if(politica==4){

    cout<<"Entre a la politica 4 readDatastr"<<endl;
    for(int i=0;*((char*)datastr+i)!='\0' && i<tamano;i++){
      cadena[i]=(char *)(datastr+pos+i);
    }
  }
  return *cadena;
}

int Interpreter::readDatanum(int *datanum, int pos,int politica){
  int valorRetornar;
  if (politica==1){//prioridad lectores

    cout<<"Entre a la politica 1 de readDatanum"<<endl;
    sem_wait(&(*(workload+1)));
    cuentalect++;
    if (cuentalect == 1){
      sem_wait(&(*(workload)));
      sem_post(&(*(workload+1)));
      valorRetornar= *(datanum+pos);
      sem_wait(&(*(workload+1)));
      cuentalect--;
    }
    if (cuentalect == 0){
      sem_post(&(*(workload)));
      sem_post(&(*(workload+1)));
    }
  }else if(politica==2){//prioridad escritores

    cout<<"Entre a la politica 2 de readDatanum"<<endl;
    sem_wait (&(*(workload+5)));
    sem_wait (&(*(workload+3)));
    sem_wait (&(*(workload+6)));
    cuentalect2++;
    if (cuentalect2 == 1){
      sem_wait (&(*(workload+4)));
      sem_post (&(*(workload+6)));
      sem_post (&(*(workload+3)));
      sem_post (&(*(workload+5)));
      valorRetornar= *(datanum+pos);
      sem_wait (&(*(workload+6)));
      cuentalect2 --;
    }
    if (cuentalect2 == 0){
      sem_post (&(*(workload+4)));
      sem_post (&(*(workload+6)));
    }

  }else if(politica==3){
    sem_wait(&(*(workload+7)));
    bloqueo--;
    cout<<"Entre a la politica 3 readDatanum------bloque: "<<bloqueo<<endl;

    valorRetornar= *(datanum+pos);
    bloqueo++;
    cout<<"bloque: "<<bloqueo<<endl;

    sem_post(&(*(workload+7)));
    return valorRetornar;
  }else if(politica==4){
    return *(datanum+pos);
  }
  return valorRetornar;
}

void Interpreter::writeDatastr(char *datastr, int pos, char* data,int politica){

  if(politica==1){
    // prioridad lectores
    sem_wait(&(*workload));
    cout<<"Entre a la politica 1 writeDatastr "<<endl;
    for(int i=0; i<strlen(data); i++){
      *(datastr + pos + i) = (char)data[i];
    }
    *(datastr + pos + strlen(data)) = '\0';
    //sleep(1000);
    sem_post(&(*workload));
  }else if(politica==2){//prioridad escritores
    sem_wait (&(*(workload+2)));
    cout<<"Entre a la politica 2 writeDatastr "<<endl;
    cuentaescr++;
    if (cuentaescr == 1){
      sem_wait(&(*(workload+3)) );
      sem_post (&(*(workload+2)));
      sem_wait (&(*(workload+4)));
      for(int i=0; i<strlen(data); i++){
        *(datastr + pos + i) = (char)data[i];
      }
      *(datastr + pos + strlen(data)) = '\0';
      sem_post (&(*(workload+4)) );
      sem_wait (&(*(workload+2)));
      cuentaescr--;
    }
    if (cuentaescr == 0){
      sem_post (&(*(workload+3)));
      sem_post(&(*(workload+2)));
    }
  }else if(politica==3){//Bloqueo
    sem_wait(&(*(workload+7)));
    bloqueo--;
    cout<<"Entre a la politica 3 writeDatastr------bloque: "<<bloqueo<<endl;
    for(int i=0; i<strlen(data); i++){
      *(datastr + pos + i) = (char)data[i];
    }
    *(datastr + pos + strlen(data)) = '\0';
    bloqueo++;
    cout<<"bloque: "<<bloqueo<<endl;
    sem_post(&(*(workload+7)));

  }  else if (politica==4){
    cout<<"Entre a la politica 4 writeDatastr"<<endl;
    for(int i=0; i<strlen(data); i++){
      *(datastr + pos + i) = (char)data[i];
    }
    *(datastr + pos + strlen(data)) = '\0';
  }else{
    cout<<"Error de segmento en writeDatastr"<<endl;
    for(int i=0; i<strlen(data); i++){
      *(datastr + pos + i) = (char)data[i];
    }
    *(datastr + pos + strlen(data)) = '\0';
  }


}

void Interpreter::writeDatanum(int *datanum, int pos, int data,int politica){
  if(politica==1){
    // prioridad lectores
    sem_wait(&(*workload));
    cout<<"Entre a la politica 1 writeDatanum"<<endl;
    *(datanum + pos) = data;
    //sleep(1000);
    sem_post(&(*workload));
  }else if(politica==2){//prioridad escritores
    sem_wait (&(*(workload+2)));
    cout<<"Entre a la politica 2 writeDatanum"<<endl;
    cuentaescr++;
    if (cuentaescr == 1){
      sem_wait(&(*(workload+3)) );
      sem_post (&(*(workload+2)));
      sem_wait (&(*(workload+4)));
      *(datanum + pos) = data;
      sem_post (&(*(workload+4)) );
      sem_wait (&(*(workload+2)));
      cuentaescr--;
    }

    if (cuentaescr == 0){
      sem_post (&(*(workload+3)));
      sem_post(&(*(workload+2)));
    }

  }else if(politica==3){//Bloqueo
    sem_wait(&(*(workload+7)));
    bloqueo--;
    cout<<"Entre a la politica 3 writeDatanum------bloque: "<<bloqueo<<endl;
    *(datanum + pos) = data;
    bloqueo++;
    cout<<"Bloqueo: "<<bloqueo<<endl;
    sem_post(&(*(workload+7)));
  }  else if(politica==4){

    cout<<"Entre a la politica 4 writeDatanum"<<endl;
    *(datanum + pos) = data;
  }else{
    cout<<"Error en segmento writeDatastr"<<endl;
  }

}

int Interpreter::interprete(int opcode, string value,int pc){
  unsigned int memref;
  unsigned int memref1;
  unsigned int memref2;
  unsigned int poslitnum,posdatanum;
  unsigned int poslitstr,posdatastr;
  unsigned int trans;
  unsigned int op,enteroDes;

  switch(opcode) {
    case 0:{

      cout << "Value: "<< value<<endl;
      //cout << "Opcode: "<<opcode<<endl;
      memref=(stoi(value.substr(1,4),0,16))>>1;
      //cout<<"memref: "<< memref<<endl;
      poslitnum=((stoi(value.substr(4,5),0,16))&131068)>>2;
      //cout <<"litnum: "<< poslitnum<<endl;
      //cout << "Es: "<< readLitnum(litnum,poslitnum) <<endl;
      int readValue = readLitnum(litnum,poslitnum);
      //cout << "datanum: " << datanum+memref << " - " << datanum << endl;

      if((inicio[0]-fin[0]!=0) && (memref>=inicioLineas[0] && memref<=finLineas[0])){
        writeDatanum(datanum, memref, readValue,1);
      }else if ((inicio[1]-fin[1]!=0) && (memref>=inicioLineas[2] && memref<=finLineas[2])){
        writeDatanum(datanum, memref, readValue,2);
      }else if ((inicio[2]-fin[2]!=0) && (memref>=inicioLineas[4] && memref<=finLineas[4])){
        writeDatanum(datanum, memref, readValue,3);
      }else if ((inicio[3]-fin[3]!=0) && (memref>=inicioLineas[6] && memref<=finLineas[6])){
        writeDatanum(datanum, memref, readValue,4);
      }else{
        cout<<"Error de segmento"<<endl;
      }

      return pc;
    }
    case 1:{
      cout << "Value: "<< value<<endl;
      //cout << "Opcode: "<<opcode<<endl;
      memref=(stoi(value.substr(1,4),0,16))>>1;
      //cout<<"memref: "<< memref<<endl;
      poslitstr=(((stoi(value.substr(4,5),0,16))&131068)>>2);
      //cout <<"litstr: "<< poslitstr<<endl;
      char* readValue = readLitstr(litstr,poslitstr,10);
      //cout<<"salida de readvalue: "<<readValue<<endl;
      //writeDatastr(datastr, memref, readValue,-1);

      if((inicio[0]-fin[0]!=0) && (memref>=inicioLineas[1] && memref<=finLineas[1])){
        writeDatastr(datastr, memref, readValue,1);
      }else if ((inicio[1]-fin[1]!=0) && (memref>=inicioLineas[3] && memref<=finLineas[3])){
        writeDatastr(datastr, memref, readValue,2);
      }else if ((inicio[2]-fin[2]!=0) && (memref>=inicioLineas[5] && memref<=finLineas[5])){
        writeDatastr(datastr, memref, readValue,3);
      }else if ((inicio[3]-fin[3]!=0) && (memref>=inicioLineas[7] && memref<=finLineas[7])){
        writeDatastr(datastr, memref, readValue,4);
      }else{
        cout<<"Error de segmento"<<endl;
      }

      return pc;
    }
    case 2:{
      cout << "Value: "<< value<<endl;
      //cout << "Opcode: "<<opcode<<endl;
      memref=(stoi(value.substr(1,4),0,16))>>1;
      //cout<<"memref: "<< memref<<endl;
      poslitnum=((stoi(value.substr(4,5),0,16))&131068)>>2;
      //cout <<"litnum: "<< poslitnum<<endl;
      int readValue = readLitnum(litnum,poslitnum);
      //writeDatanum(datanum,memref,(readValue+pc),-1);
      if((inicio[0]-fin[0]!=0) && (memref>=inicioLineas[0] && memref<=finLineas[0])){
        writeDatanum(datanum, memref, readValue,1);
      }else if ((inicio[1]-fin[1]!=0) && (memref>=inicioLineas[2] && memref<=finLineas[2])){
        writeDatanum(datanum, memref, readValue,2);
      }else if ((inicio[2]-fin[2]!=0) && (memref>=inicioLineas[4] && memref<=finLineas[4])){
        writeDatanum(datanum, memref, readValue,3);
      }else if ((inicio[3]-fin[3]!=0) && (memref>=inicioLineas[6] && memref<=finLineas[6])){
        writeDatanum(datanum, memref, readValue,4);
      }else{
        cout<<"Error de segmento"<<endl;
      }
      return pc;
    }
    case 3:{
      cout << "Value: "<< value<<endl;
      //cout << "Opcode: "<<opcode<<endl;
      memref=(stoi(value.substr(1,4),0,16))>>1;
      //cout<<"memref: "<< memref<<endl;
      //int readValue = readDatanum(datanum,memref,-1);
      int readValue;
      if((inicio[0]-fin[0]!=0) && (memref>=inicioLineas[0] && memref<=finLineas[0])){
        readValue = readDatanum(datanum,memref,1);
      }else if ((inicio[1]-fin[1]!=0) && (memref>=inicioLineas[2] && memref<=finLineas[2])){
        readValue = readDatanum(datanum,memref,2);
      }else if ((inicio[2]-fin[2]!=0) && (memref>=inicioLineas[4] && memref<=finLineas[4])){
        readValue = readDatanum(datanum,memref,3);
      }else if ((inicio[3]-fin[3]!=0) && (memref>=inicioLineas[6] && memref<=finLineas[6])){
        readValue = readDatanum(datanum,memref,4);
      }else{
        cout<<"Error de segmento"<<endl;
      }
      return (readValue-1);
    }
    case 4:{
      cout << "Value: "<< value<<endl;
      //cout << "Opcode: "<<opcode<<endl;
      trans=((stoi(value.substr(1,1),0,16))&8)>>3;
      //cout<<"transf: "<< trans<<endl;
      memref=(stoi(value.substr(1,4),0,16))&32767;
      //cout<<"memref: "<< memref<<endl;
      if (trans){
          posdatanum=(stoi(value.substr(5,4),0,16)>>1);
          //cout <<"litnum: "<< posdatanum<<endl;
          //int readValue = readDatanum(datanum,posdatanum,-1);
          int readValue;
          if((inicio[0]-fin[0]!=0) && (posdatanum>=inicioLineas[0] && posdatanum<=finLineas[0])){
            readValue = readDatanum(datanum,posdatanum,1);
          }else if ((inicio[1]-fin[1]!=0) && (posdatanum>=inicioLineas[2] && posdatanum<=finLineas[2])){
            readValue = readDatanum(datanum,posdatanum,2);
          }else if ((inicio[2]-fin[2]!=0) && (posdatanum>=inicioLineas[4] && posdatanum<=finLineas[4])){
            readValue = readDatanum(datanum,posdatanum,3);
          }else if ((inicio[3]-fin[3]!=0) && (posdatanum>=inicioLineas[6] && posdatanum<=finLineas[6])){
            readValue = readDatanum(datanum,posdatanum,4);
          }else{
            cout<<"Error de segmento"<<endl;
          }

          //writeDatanum(datanum,memref,readValue,-1);
          if((inicio[0]-fin[0]!=0) && (memref>=inicioLineas[0] && memref<=finLineas[0])){
            writeDatanum(datanum, memref, readValue,1);
          }else if ((inicio[1]-fin[1]!=0) && (memref>=inicioLineas[2] && memref<=finLineas[2])){
            writeDatanum(datanum, memref, readValue,2);
          }else if ((inicio[2]-fin[2]!=0) && (memref>=inicioLineas[4] && memref<=finLineas[4])){
            writeDatanum(datanum, memref, readValue,3);
          }else if ((inicio[3]-fin[3]!=0) && (memref>=inicioLineas[6] && memref<=finLineas[6])){
            writeDatanum(datanum, memref, readValue,4);
          }else{
            cout<<"Error de segmento"<<endl;
          }
      }else{
          posdatastr=(stoi(value.substr(5,4),0,16)>>1);
          //cout <<"litstr: "<< posdatastr<<endl;
          //char* readValue = readDatastr(datastr,posdatastr,128,-1);
          char* readValue;
          if((inicio[0]-fin[0]!=0) && (posdatastr>=inicioLineas[0] && posdatastr<=finLineas[0])){
            readValue = readDatastr(datastr,posdatastr,128,1);
          }else if ((inicio[1]-fin[1]!=0) && (posdatastr>=inicioLineas[2] && posdatastr<=finLineas[2])){
            readValue = readDatastr(datastr,posdatastr,128,2);
          }else if ((inicio[2]-fin[2]!=0) && (posdatastr>=inicioLineas[4] && posdatastr<=finLineas[4])){
            readValue = readDatastr(datastr,posdatastr,128,3);
          }else if ((inicio[3]-fin[3]!=0) && (posdatastr>=inicioLineas[6] && posdatastr<=finLineas[6])){
            readValue = readDatastr(datastr,posdatastr,128,4);
          }else{
            cout<<"Error de segmento"<<endl;
          }
          //writeDatastr(datastr,memref,readValue,-1);
          if((inicio[0]-fin[0]!=0) && (memref>=inicioLineas[1] && memref<=finLineas[1])){
            writeDatastr(datastr, memref, readValue,1);
          }else if ((inicio[1]-fin[1]!=0) && (memref>=inicioLineas[3] && memref<=finLineas[3])){
            writeDatastr(datastr, memref, readValue,2);
          }else if ((inicio[2]-fin[2]!=0) && (memref>=inicioLineas[5] && memref<=finLineas[5])){
            writeDatastr(datastr, memref, readValue,3);
          }else if ((inicio[3]-fin[3]!=0) && (memref>=inicioLineas[7] && memref<=finLineas[7])){
            writeDatastr(datastr, memref, readValue,4);
          }else{
            cout<<"Error de segmento"<<endl;
          }
      }
      return pc;
    }
    case 5:{
      cout << "Value: "<< value<<endl;
      //cout << "Opcode: "<<opcode<<endl;
      op=(stoi(value.substr(1,1),0,16))>>1;
      //cout<<"operation: "<< op<<endl;
      trans=(stoi(value.substr(1,1),0,16))&1;
      //cout<<"transf: "<< trans<<endl;
      memref=(stoi(value.substr(2,4),0,16))>>1;
      //cout<<"memref: "<< memref<<endl;
      memref1=((stoi(value.substr(5,5),0,16))&131068)>>2;
      memref2=((stoi(value.substr(9,5),0,16))&262136)>>3;
      //cout<<"memref1: "<< memref1<<endl;
      //cout<<"memref2: "<< memref2<<endl;
      if (trans){
          int result;
          //int readValue1 = readDatanum(datanum,memref1,-1);
          int readValue1;
          if((inicio[0]-fin[0]!=0) && (memref1>=inicioLineas[0] && memref1<=finLineas[0])){
            readValue1 = readDatanum(datanum,memref1,1);
          }else if ((inicio[1]-fin[1]!=0) && (memref1>=inicioLineas[2] && memref1<=finLineas[2])){
            readValue1 = readDatanum(datanum,memref1,2);
          }else if ((inicio[2]-fin[2]!=0) && (memref1>=inicioLineas[4] && memref1<=finLineas[4])){
            readValue1 = readDatanum(datanum,memref1,3);
          }else if ((inicio[3]-fin[3]!=0) && (memref1>=inicioLineas[6] && memref1<=finLineas[6])){
            readValue1 = readDatanum(datanum,memref1,4);
          }else{
            cout<<"Error de segmento"<<endl;
          }
          //int readValue2 = readDatanum(datanum,memref2,-1);
          int readValue2;
          if((inicio[0]-fin[0]!=0) && (memref2>=inicioLineas[0] && memref2<=finLineas[0])){
            readValue2 = readDatanum(datanum,memref2,1);
          }else if ((inicio[1]-fin[1]!=0) && (memref2>=inicioLineas[2] && memref2<=finLineas[2])){
            readValue2 = readDatanum(datanum,memref2,2);
          }else if ((inicio[2]-fin[2]!=0) && (memref2>=inicioLineas[4] && memref2<=finLineas[4])){
            readValue2 = readDatanum(datanum,memref2,3);
          }else if ((inicio[3]-fin[3]!=0) && (memref2>=inicioLineas[6] && memref2<=finLineas[6])){
            readValue2 = readDatanum(datanum,memref2,4);
          }else{
            cout<<"Error de segmento"<<endl;
          }
        if(op==0){
          //int readValue1 = readDatanum(datanum,memref1,-1);
          //int readValue2 = readDatanum(datanum,memref2,-1);

          //cout<<"valor1: "<<readValue1<<" valor2: "<<readValue2<<endl;
          result= readValue1+readValue2;
          //writeDatanum(datanum,memref,result,-1);
        }else if(op==1){
          // int readValue1 = readDatanum(datanum,memref1,-1);
          // int readValue2 = readDatanum(datanum,memref2,-1);
          result= readValue1-readValue2;
          //writeDatanum(datanum,memref,result,-1);

        }else if(op==2){
          // int readValue1 = readDatanum(datanum,memref1,-1);
          // int readValue2 = readDatanum(datanum,memref2,-1);
          result= readValue1*readValue2;
          //writeDatanum(datanum,memref,result,-1);

        }else if(op==3){
          // int readValue1 = readDatanum(datanum,memref1,-1);
          // int readValue2 = readDatanum(datanum,memref2,-1);
          result= readValue1/readValue2;
          //writeDatanum(datanum,memref,result,-1);

        }else if(op==4){
          //int readValue1 = readDatanum(datanum,memref1,-1);
          //int readValue2 = readDatanum(datanum,memref2,-1);
          result= readValue1%readValue2;
          //writeDatanum(datanum,memref,result,-1);

        }
        //writeDatanum(datanum,memref,result,-1);
        if((inicio[0]-fin[0]!=0) && (memref>=inicioLineas[0] && memref<=finLineas[0])){
          writeDatanum(datanum, memref, result,1);
        }else if ((inicio[1]-fin[1]!=0) && (memref>=inicioLineas[2] && memref<=finLineas[2])){
          writeDatanum(datanum, memref, result,2);
        }else if ((inicio[2]-fin[2]!=0) && (memref>=inicioLineas[4] && memref<=finLineas[4])){
          writeDatanum(datanum, memref, result,3);
        }else if ((inicio[3]-fin[3]!=0) && (memref>=inicioLineas[6] && memref<=finLineas[6])){
          writeDatanum(datanum, memref, result,4);
        }else{
          cout<<"Error de segmento"<<endl;
        }

      }else{
        char* readValue1;// = readDatastr(datastr,memref1,128,-1);
        if((inicio[0]-fin[0]!=0) && (memref1>=inicioLineas[1] && memref1<=finLineas[1])){
          readValue1 = readDatastr(datastr,memref1,128,1);
        }else if ((inicio[1]-fin[1]!=0) && (memref1>=inicioLineas[3] && memref1<=finLineas[3])){
          readValue1 = readDatastr(datastr,memref1,128,2);
        }else if ((inicio[2]-fin[2]!=0) && (memref1>=inicioLineas[5] && memref1<=finLineas[5])){
          readValue1 = readDatastr(datastr,memref1,128,3);
        }else if ((inicio[3]-fin[3]!=0) && (memref1>=inicioLineas[7] && memref1<=finLineas[7])){
          readValue1 = readDatastr(datastr,memref1,128,4);
        }else{
          cout<<"Error de segmento"<<endl;
        }
        //char* readValue2 = readDatastr(datastr,memref2,128,-1);
        char* readValue2;
        if((inicio[0]-fin[0]!=0) && (memref2>=inicioLineas[1] && memref2<=finLineas[1])){
          readValue2 = readDatastr(datastr,memref2,128,1);
        }else if ((inicio[1]-fin[1]!=0) && (memref2>=inicioLineas[3] && memref2<=finLineas[3])){
          readValue2 = readDatastr(datastr,memref2,128,2);
        }else if ((inicio[2]-fin[2]!=0) && (memref2>=inicioLineas[5] && memref2<=finLineas[5])){
          readValue2 = readDatastr(datastr,memref2,128,3);
        }else if ((inicio[3]-fin[3]!=0) && (memref2>=inicioLineas[7] && memref2<=finLineas[7])){
          readValue2 = readDatastr(datastr,memref2,128,4);
        }else{
          cout<<"Error de segmento"<<endl;
        }
        string cadena= (string)(readValue1);
        string cadena1= (string)(readValue2);
        string mayor;
        string result;
        int longitud;
        char* result2;

        if (op==0){
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
          result2= (char*)result.c_str();
          //writeDatastr(datastr,memref,result2,-1  );

        }else if(op==1){
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
          result2= (char*)result.c_str();
          //writeDatastr(datastr,memref,result2,-1);

        }else if(op==2){
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
          result2= (char*)result.c_str();
          //writeDatastr(datastr,memref,result2,-1);

        }else if(op==3){
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
          result2= (char*)result.c_str();
          //writeDatastr(datastr,memref,result2,-1);

        }else if(op==4){
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
          result2= (char*)result.c_str();
          //writeDatastr(datastr,memref,result2,-1);


        }
      //writeDatastr(datastr,memref,result2,-1);
        if((inicio[0]-fin[0]!=0) && (memref>=inicioLineas[1] && memref<=finLineas[1])){
          writeDatastr(datastr,memref,result2,1);
        }else if ((inicio[1]-fin[1]!=0) && (memref>=inicioLineas[3] && memref<=finLineas[3])){
          writeDatastr(datastr,memref,result2,2);
        }else if ((inicio[2]-fin[2]!=0) && (memref>=inicioLineas[5] && memref<=finLineas[5])){
          writeDatastr(datastr,memref,result2,3);
        }else if ((inicio[3]-fin[3]!=0) && (memref>=inicioLineas[7] && memref<=finLineas[7])){
          writeDatastr(datastr,memref,result2,4);
        }else{
          cout<<"Error de segmento"<<endl;
        }
      }
      return pc;
    }
    case 6:{
      cout << "Value: "<< value<<endl;
      //cout << "Opcode: "<<opcode<<endl;
      trans=((stoi(value.substr(1,1),0,16))&8)>>3;
      //cout<<"transf: "<< trans<<endl;
      memref=(stoi(value.substr(1,4),0,16))&32767;
      //cout<<"memref: "<< memref<<endl;
      if (trans){
          posdatanum=(stoi(value.substr(5,4),0,16)>>1);
          //cout <<"litnum: "<< posdatanum<<endl;
          enteroDes=(stoi(value.substr(8,5),0,16)&131068);

          int readValue=readLitnum(litnum,enteroDes);
          int readValue2;//= readDatanum(datanum,readValue+posdatanum,-1);

          if((inicio[0]-fin[0]!=0) && (readValue+posdatanum>=inicioLineas[0] && readValue+posdatanum<=finLineas[0])){
            readValue2 = readDatanum(datanum,readValue+posdatanum,1);
          }else if ((inicio[1]-fin[1]!=0) && (readValue+posdatanum>=inicioLineas[2] && readValue+posdatanum<=finLineas[2])){
            readValue2 = readDatanum(datanum,readValue+posdatanum,2);
          }else if ((inicio[2]-fin[2]!=0) && (readValue+posdatanum>=inicioLineas[4] && readValue+posdatanum<=finLineas[4])){
            readValue2 = readDatanum(datanum,readValue+posdatanum,3);
          }else if ((inicio[3]-fin[3]!=0) && (readValue+posdatanum>=inicioLineas[6] && readValue+posdatanum<=finLineas[6])){
            readValue2 = readDatanum(datanum,readValue+posdatanum,4);
          }else{
            cout<<"Error de segmento"<<endl;
          }

          //writeDatanum(datanum,memref,readValue2,-1);
          if((inicio[0]-fin[0]!=0) && (memref>=inicioLineas[0] && memref<=finLineas[0])){
            writeDatanum(datanum, memref, readValue2,1);
          }else if ((inicio[1]-fin[1]!=0) && (memref>=inicioLineas[2] && memref<=finLineas[2])){
            writeDatanum(datanum, memref, readValue2,2);
          }else if ((inicio[2]-fin[2]!=0) && (memref>=inicioLineas[4] && memref<=finLineas[4])){
            writeDatanum(datanum, memref, readValue2,3);
          }else if ((inicio[3]-fin[3]!=0) && (memref>=inicioLineas[6] && memref<=finLineas[6])){
            writeDatanum(datanum, memref, readValue2,4);
          }else{
            cout<<"Error de segmento"<<endl;
          }
      }else{
          posdatastr=(stoi(value.substr(5,4),0,16)>>1);
          //cout <<"litstr: "<< posdatastr<<endl;
          enteroDes=(stoi(value.substr(8,5),0,16)&131068);
          int readValue=readLitnum(litnum,enteroDes);
          char* readValue2;// = readDatastr(datastr,posdatastr+readValue,128,-1);

          if((inicio[0]-fin[0]!=0) && (posdatastr+readValue>=inicioLineas[1] && posdatastr+readValue<=finLineas[1])){
            readValue2 = readDatastr(datastr,posdatastr+readValue,128,1);
          }else if ((inicio[1]-fin[1]!=0) && (posdatastr+readValue>=inicioLineas[3] && posdatastr+readValue<=finLineas[3])){
            readValue2 = readDatastr(datastr,posdatastr+readValue,128,2);
          }else if ((inicio[2]-fin[2]!=0) && (posdatastr+readValue>=inicioLineas[5] && posdatastr+readValue<=finLineas[5])){
            readValue2 = readDatastr(datastr,posdatastr+readValue,128,3);
          }else if ((inicio[3]-fin[3]!=0) && (posdatastr+readValue>=inicioLineas[7] && posdatastr+readValue<=finLineas[7])){
            readValue2 = readDatastr(datastr,posdatastr+readValue,128,4);
          }else{
            cout<<"Error de segmento"<<endl;
          }

          //writeDatastr(datastr,memref,readValue2,-1);
          if((inicio[0]-fin[0]!=0) && (memref>=inicioLineas[1] && memref<=finLineas[1])){
            writeDatastr(datastr,memref,readValue2,1);
          }else if ((inicio[1]-fin[1]!=0) && (memref>=inicioLineas[3] && memref<=finLineas[3])){
            writeDatastr(datastr,memref,readValue2,2);
          }else if ((inicio[2]-fin[2]!=0) && (memref>=inicioLineas[5] && memref<=finLineas[5])){
            writeDatastr(datastr,memref,readValue2,3);
          }else if ((inicio[3]-fin[3]!=0) && (memref>=inicioLineas[7] && memref<=finLineas[7])){
            writeDatastr(datastr,memref,readValue2,4);
          }else{
            cout<<"Error de segmento"<<endl;
          }
      }
      return pc;
    }
    case 7:{
      cout << "Value: "<< value<<endl;
      //cout << "Opcode: "<<opcode<<endl;
      trans=((stoi(value.substr(1,1),0,16))&8)>>3;
      //cout<<"transf: "<< trans<<endl;
      memref=(stoi(value.substr(1,4),0,16))&32767;
      //cout<<"memref: "<< memref<<endl;
      if (trans){
          enteroDes=(stoi(value.substr(5,4),0,16)>>1);
          posdatanum=(stoi(value.substr(8,5),0,16)&131068);
          //cout <<"litnum: "<< posdatanum<<endl;
          int readValue=readLitnum(litnum,enteroDes);
          //int readValue2=readDatanum(datanum,posdatanum,-1);

          int readValue2;//= readDatanum(datanum,readValue+posdatanum,-1);

          if((inicio[0]-fin[0]!=0) && (posdatanum>=inicioLineas[0] && posdatanum<=finLineas[0])){
            readValue2 = readDatanum(datanum,posdatanum,1);
          }else if ((inicio[1]-fin[1]!=0) && (posdatanum>=inicioLineas[2] && posdatanum<=finLineas[2])){
            readValue2 = readDatanum(datanum,posdatanum,2);
          }else if ((inicio[2]-fin[2]!=0) && (posdatanum>=inicioLineas[4] && posdatanum<=finLineas[4])){
            readValue2 = readDatanum(datanum,posdatanum,3);
          }else if ((inicio[3]-fin[3]!=0) && (posdatanum>=inicioLineas[6] && posdatanum<=finLineas[6])){
            readValue2 = readDatanum(datanum,posdatanum,4);
          }else{
            cout<<"Error de segmento"<<endl;
          }

          //writeDatanum(datanum,memref+readValue,readValue2,-1);
          if((inicio[0]-fin[0]!=0) && (memref+readValue>=inicioLineas[0] && memref+readValue<=finLineas[0])){
            writeDatanum(datanum, memref+readValue, readValue2,1);
          }else if ((inicio[1]-fin[1]!=0) && (memref+readValue>=inicioLineas[2] && memref+readValue<=finLineas[2])){
            writeDatanum(datanum, memref+readValue, readValue2,2);
          }else if ((inicio[2]-fin[2]!=0) && (memref+readValue>=inicioLineas[4] && memref+readValue<=finLineas[4])){
            writeDatanum(datanum, memref+readValue, readValue2,3);
          }else if ((inicio[3]-fin[3]!=0) && (memref+readValue>=inicioLineas[6] && memref+readValue<=finLineas[6])){
            writeDatanum(datanum, memref+readValue, readValue2,4);
          }else{
            cout<<"Error de segmento"<<endl;
          }
      }else{
          enteroDes=(stoi(value.substr(5,4),0,16)>>1);
          posdatastr=(stoi(value.substr(8,5),0,16)&131068);
          //cout <<"litstr: "<< posdatastr<<endl;
          int readValue=readLitnum(litnum,enteroDes);
          char* readValue2;// = readDatastr(datastr,posdatastr,128,-1);
          if((inicio[0]-fin[0]!=0) && (posdatastr>=inicioLineas[1] && posdatastr<=finLineas[1])){
            readValue2 = readDatastr(datastr,posdatastr,128,1);
          }else if ((inicio[1]-fin[1]!=0) && (posdatastr>=inicioLineas[3] && posdatastr<=finLineas[3])){
            readValue2 = readDatastr(datastr,posdatastr,128,2);
          }else if ((inicio[2]-fin[2]!=0) && (posdatastr>=inicioLineas[5] && posdatastr<=finLineas[5])){
            readValue2 = readDatastr(datastr,posdatastr,128,3);
          }else if ((inicio[3]-fin[3]!=0) && (posdatastr>=inicioLineas[7] && posdatastr<=finLineas[7])){
            readValue2 = readDatastr(datastr,posdatastr,128,4);
          }else{
            cout<<"Error de segmento"<<endl;
          }

          //writeDatastr(datastr,memref+readValue,readValue2,-1);
          if((inicio[0]-fin[0]!=0) && (memref+readValue>=inicioLineas[1] && memref+readValue<=finLineas[1])){
            writeDatastr(datastr,memref+readValue,readValue2,1);
          }else if ((inicio[1]-fin[1]!=0) && (memref+readValue>=inicioLineas[3] && memref+readValue<=finLineas[3])){
            writeDatastr(datastr,memref+readValue,readValue2,2);
          }else if ((inicio[2]-fin[2]!=0) && (memref+readValue>=inicioLineas[5] && memref+readValue<=finLineas[5])){
            writeDatastr(datastr,memref+readValue,readValue2,3);
          }else if ((inicio[3]-fin[3]!=0) && (memref+readValue>=inicioLineas[7] && memref+readValue<=finLineas[7])){
            writeDatastr(datastr,memref+readValue,readValue2,4);
          }else{
            cout<<"Error de segmento"<<endl;
          }
      }
      return pc;
    }
    case 8:{
      cout << "Value: "<< value<<endl;
      //cout << "Opcode: "<<opcode<<endl;
      memref=(stoi(value.substr(1,4),0,16))>>1;
      //cout<<"memref: "<< memref<<endl;
      int readValue;
      cout<<"Ingrese un entero: ";
      cin>>readValue;
      //writeDatanum(datanum,memref,readValue,-1);
      if((inicio[0]-fin[0]!=0) && (memref>=inicioLineas[0] && memref<=finLineas[0])){
        writeDatanum(datanum, memref, readValue,1);
      }else if ((inicio[1]-fin[1]!=0) && (memref>=inicioLineas[2] && memref<=finLineas[2])){
        writeDatanum(datanum, memref, readValue,2);
      }else if ((inicio[2]-fin[2]!=0) && (memref>=inicioLineas[4] && memref<=finLineas[4])){
        writeDatanum(datanum, memref, readValue,3);
      }else if ((inicio[3]-fin[3]!=0) && (memref>=inicioLineas[6] && memref<=finLineas[6])){
        writeDatanum(datanum, memref, readValue,4);
      }else{
        cout<<"Error de segmento"<<endl;
      }
      return pc;
    }
    case 9:{
      cout << "Value: "<< value<<endl;
      //cout << "Opcode: "<<opcode<<endl;
      memref=(stoi(value.substr(1,4),0,16))>>1;
      //cout<<"memref: "<< memref<<endl;
      //int readValue=readDatanum(datanum,memref,-1);
      int readValue;
      if((inicio[0]-fin[0]!=0) && (memref>=inicioLineas[0] && memref<=finLineas[0])){
        readValue = readDatanum(datanum,memref,1);
      }else if ((inicio[1]-fin[1]!=0) && (memref>=inicioLineas[2] && memref<=finLineas[2])){
        readValue = readDatanum(datanum,memref,2);
      }else if ((inicio[2]-fin[2]!=0) && (memref>=inicioLineas[4] && memref<=finLineas[4])){
        readValue = readDatanum(datanum,memref,3);
      }else if ((inicio[3]-fin[3]!=0) && (memref>=inicioLineas[6] && memref<=finLineas[6])){
        readValue = readDatanum(datanum,memref,4);
      }else{
        cout<<"Error de segmento"<<endl;
      }
      cout<<"writeInt: "<<readValue<<endl;

      return pc;

    }
    case 10:{
      cout << "Value: "<< value<<endl;
      //cout << "Opcode: "<<opcode<<endl;
      memref=(stoi(value.substr(1,4),0,16))>>1;
      //cout<<"memref: "<< memref<<endl;
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
      //writeDatastr(datastr,memref, valor2,-1);
      if((inicio[0]-fin[0]!=0) && (memref>=inicioLineas[1] && memref<=finLineas[1])){
        writeDatastr(datastr,memref,valor2,1);
      }else if ((inicio[1]-fin[1]!=0) && (memref>=inicioLineas[3] && memref<=finLineas[3])){
        writeDatastr(datastr,memref,valor2,2);
      }else if ((inicio[2]-fin[2]!=0) && (memref>=inicioLineas[5] && memref<=finLineas[5])){
        writeDatastr(datastr,memref,valor2,3);
      }else if ((inicio[3]-fin[3]!=0) && (memref>=inicioLineas[7] && memref<=finLineas[7])){
        writeDatastr(datastr,memref,valor2,4);
      }else{
        cout<<"Error de segmento"<<endl;
      }
      return pc;
    }
    case 11:{
      cout << "Value: "<< value<<endl;
      //cout << "Opcode: "<<opcode<<endl;
      memref=(stoi(value.substr(1,4),0,16))>>1;
      //cout<<"memref: "<< memref<<endl;
      //char* readValue=readDatastr(datastr,memref,128,-1);
      char* readValue;// = readDatastr(datastr,memref1,128,-1);
      if((inicio[0]-fin[0]!=0) && (memref>=inicioLineas[1] && memref<=finLineas[1])){
        readValue = readDatastr(datastr,memref,128,1);
      }else if ((inicio[1]-fin[1]!=0) && (memref>=inicioLineas[3] && memref<=finLineas[3])){
        readValue = readDatastr(datastr,memref,128,2);
      }else if ((inicio[2]-fin[2]!=0) && (memref>=inicioLineas[5] && memref<=finLineas[5])){
        readValue = readDatastr(datastr,memref,128,3);
      }else if ((inicio[3]-fin[3]!=0) && (memref>=inicioLineas[7] && memref<=finLineas[7])){
        readValue = readDatastr(datastr,memref,128,4);
      }else{
        cout<<"Error de segmento"<<endl;
      }
      cout<<"writeStr: "<<readValue<<endl;
      return pc;
    }
    case 12:{
      cout << "Value: "<< value<<endl;
      //cout << "Opcode: "<<opcode<<endl;
      memref=(stoi(value.substr(1,4),0,16))>>1;
      int readValue=readLitnum(litnum,memref);
      pc=readValue;
      return pc;
    }
    case 13:{
      cout << "Value: "<< value<<endl;
      //cout << "Opcode: "<<opcode<<endl;
      op=(stoi(value.substr(1,1),0,16))>>1;
      //cout<<"operation: "<< op<<endl;
      trans=(stoi(value.substr(1,1),0,16))&1;
      //cout<<"transf: "<< trans<<endl;
      memref=(stoi(value.substr(2,4),0,16))>>1;
      //cout<<"memref: "<< memref<<endl;
      memref1=((stoi(value.substr(5,5),0,16))&131068)>>2;
      memref2=((stoi(value.substr(9,5),0,16))&262136)>>3;
      //cout<<"memref1: "<< memref1<<endl;
      //cout<<"memref2: "<< memref2<<endl;

      if (trans){

        //int readValue=readDatanum(datanum,memref,-1);
        int readValue;
        if((inicio[0]-fin[0]!=0) && (memref>=inicioLineas[0] && memref<=finLineas[0])){
          readValue = readDatanum(datanum,memref,1);
        }else if ((inicio[1]-fin[1]!=0) && (memref>=inicioLineas[2] && memref<=finLineas[2])){
          readValue = readDatanum(datanum,memref,2);
        }else if ((inicio[2]-fin[2]!=0) && (memref>=inicioLineas[4] && memref<=finLineas[4])){
          readValue = readDatanum(datanum,memref,3);
        }else if ((inicio[3]-fin[3]!=0) && (memref>=inicioLineas[6] && memref<=finLineas[6])){
          readValue = readDatanum(datanum,memref,4);
        }else{
          cout<<"Error de segmento"<<endl;
        }
        //int readValue2=readDatanum(datanum,memref1,-1);
        int readValue2;
        if((inicio[0]-fin[0]!=0) && (memref1>=inicioLineas[0] && memref1<=finLineas[0])){
          readValue2 = readDatanum(datanum,memref1,1);
        }else if ((inicio[1]-fin[1]!=0) && (memref1>=inicioLineas[2] && memref1<=finLineas[2])){
          readValue2 = readDatanum(datanum,memref1,2);
        }else if ((inicio[2]-fin[2]!=0) && (memref1>=inicioLineas[4] && memref1<=finLineas[4])){
          readValue2 = readDatanum(datanum,memref1,3);
        }else if ((inicio[3]-fin[3]!=0) && (memref1>=inicioLineas[6] && memref1<=finLineas[6])){
          readValue2 = readDatanum(datanum,memref1,4);
        }else{
          cout<<"Error de segmento"<<endl;
        }

        if (op==0){//>=
          if(readValue>=readValue2){
            pc=readLitnum(litnum,memref2);
          }
        }else if(op==1){
          if(readValue>readValue2){
            pc=readLitnum(litnum,memref2);
          }
        }else if(op==2){
          if(readValue<=readValue2){
            pc=readLitnum(litnum,memref2);
          }
        }else if (op==3){
          if(readValue<readValue2){
            pc=readLitnum(litnum,memref2);
          }
        }else if(op==4){
          if(readValue==readValue2){
            pc=readLitnum(litnum,memref2);
          }
        }else if(op==5){
          if(readValue!=readValue2){
            pc=readLitnum(litnum,memref2);
          }
        }
      }else{//char* readDatastr(char *datastr, int pos, int tamano)

        //char* readValue=readDatastr(datastr,memref,1,-1);
        char* readValue;
        if((inicio[0]-fin[0]!=0) && (memref>=inicioLineas[0] && memref<=finLineas[0])){
          readValue = readDatastr(datastr,memref,128,1);
        }else if ((inicio[1]-fin[1]!=0) && (memref>=inicioLineas[2] && memref<=finLineas[2])){
          readValue = readDatastr(datastr,memref,128,2);
        }else if ((inicio[2]-fin[2]!=0) && (memref>=inicioLineas[4] && memref<=finLineas[4])){
          readValue = readDatastr(datastr,memref,128,3);
        }else if ((inicio[3]-fin[3]!=0) && (memref>=inicioLineas[6] && memref<=finLineas[6])){
          readValue = readDatastr(datastr,memref,128,4);
        }else{
          cout<<"Error de segmento"<<endl;
        }
        //char* readValue2=readDatastr(datastr,memref1,1,-1);
        char* readValue2;
        if((inicio[0]-fin[0]!=0) && (memref1>=inicioLineas[0] && memref1<=finLineas[0])){
          readValue2 = readDatastr(datastr,memref1,128,1);
        }else if ((inicio[1]-fin[1]!=0) && (memref1>=inicioLineas[2] && memref1<=finLineas[2])){
          readValue2 = readDatastr(datastr,memref1,128,2);
        }else if ((inicio[2]-fin[2]!=0) && (memref1>=inicioLineas[4] && memref1<=finLineas[4])){
          readValue2 = readDatastr(datastr,memref1,128,3);
        }else if ((inicio[3]-fin[3]!=0) && (memref1>=inicioLineas[6] && memref1<=finLineas[6])){
          readValue2 = readDatastr(datastr,memref1,128,4);
        }else{
          cout<<"Error de segmento"<<endl;
        }

        if (op==0){//>=
          if(readValue[0]>=readValue2[0]){
            pc=readLitnum(litnum,memref2);
          }
        }else if (op==1){
          if(readValue[0]>readValue2[0]){
            pc=readLitnum(litnum,memref2);
          }
        }else if(op==2){
          if(readValue[0]<=readValue2[0]){
            pc=readLitnum(litnum,memref2);
          }
        }else if (op==3){
          if(readValue[0]<readValue2[0]){
            pc=readLitnum(litnum,memref2);
          }
        }else if (op==4){
          if(readValue[0]==readValue2[0]){
            pc=readLitnum(litnum,memref2);
          }
        }else if (op==5){
          if(readValue[0]!=readValue2[0]){
            pc=readLitnum(litnum,memref2);
          }
        }
      }
      return pc;
    }
    case 14:{
      cout << "Value: "<< value<<endl;
      //cout << "Opcode: "<<opcode<<endl;
      pc=-1;
      return pc;
    }
    case 15:{
      cout << "Value: "<< value<<endl;
      //cout << "Opcode: "<<opcode<<endl;
      pc=-2;
      return pc;
    }
    return pc;
  }

}

void Interpreter::proceso(string file){
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
      //
      // int politica1 = *(memg+6);
      // int inicio1 = politica1>>16;
      // int fin1 = politica1&65535;
      //
      // unsigned int posPolitica1 = *(memg+inicio1);
      // int segmento1=posPolitica1>>31;
      // int num=(posPolitica1>>16)&32767;
      // int num1=posPolitica1&32767;


      // prioridad lectores, prioridad escritores, bloqueo, libre
      for (int i=0;i<4;i++){
        politicas[i]=*(memg+6+i);
        inicio[i] = politicas[i]>>16;
        fin[i] = politicas[i]&65535;

      }
      // politicas = {*(memg+6), *(memg+7), *(memg+8), *(memg+9)};
      // inicio = {politicas[0]>>16,politicas[1]>>16,politicas[2]>>16,politicas[3]>>16};
      // fin = {politicas[0]&65535,politicas[1]&65535,politicas[2]&65535,politicas[3]&65535};
      // nroLineas= {fin[0]-inicio[0],fin[1]-inicio[1],fin[2]-inicio[2],fin[3]-inicio[3]};

      for (int i=0;i<8;i++){
        inicioLineas[i]=-1;
        finLineas[i]=-1;
      }
      for(int i=0; i<4; i++){
        for(int j=inicio[i]; j<fin[i]; j++){
          int val= *(memg+j)>>31;
          if(val==-1){
            inicioLineas[i*2]=(*(memg+j)>>16)&32767;
            finLineas[i*2]=(*(memg+j))&32767;
          }else if(val==0){
            inicioLineas[i*2+1]=(*(memg+j)>>16)&32767;
            finLineas[i*2+1]=(*(memg+j))&32767;
          }
          //cout << "inicio: " << inicioLineas[i*2] << " fin: " << finLineas[i*2+1] << endl;

        }
      }
      /*for(int i=0; i<8; i++){
        cout << "inicioLineas: " << inicioLineas[i] << " finLineas: " << finLineas[i] << endl;
      }*/

      for(int i = 0; i < instructions.size() ; i++)
      {
        int valor=i;
        int respuesta;
        opcode = stoi(instructions[i].substr(0,1),0,16);

        i = interprete(opcode, instructions[i],i);
        if (i==-1){
          cout <<"Pare por halt"<<endl;
          i=instructions.size();
        }
        if (i==-2){
          cout <<"Pare por break"<<endl;
          cout<<"Quiere continuar? (si=1,no=0)"<<endl;
          cin>>respuesta;
          if(respuesta){
              i=valor;
          }else{
            i=instructions.size();
          }
        }

      }
  }
}
