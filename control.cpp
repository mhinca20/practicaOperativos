#include "control.h"
#include "interpreter.h"

Control::Control(){}

Control::~Control() {}

void Control::writeMemg(string nameFile,int limit, int *memg,int *litnum,int limiteNum,int *litstr,int limiteStr) {
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

char* Control::createMemory(int max, char* name){
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

int Control::mainMemory(char* memoryName, string fileName, Control* c){
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

  char *pMem = c->createMemory(max,memoryName);
  memg = (int *)pMem;
  litnum = (int *)pMem+base[1];
  litstr = (int *)pMem+base[2];
  datanum = (int *)pMem+base[3];
  datastr = (char*)((int *)pMem+base[4]);
  workload = (sem_t*)((int *)pMem+base[5]);

  // ejemplo semaforos
  sem_t escr, mutex1, mutexcescr, slect, sescr, mutex2, mutextlect, mutexB;
  sem_init(&mutex1,0,1);
  sem_init(&escr,0,1);
  sem_init(&mutexcescr,0,1);
  sem_init(&slect,0,1);
  sem_init(&sescr,0,1);
  sem_init(&mutex2,0,1);
  sem_init(&mutextlect,0,1);
  sem_init(&mutexB,0,1);
  // prioridad lectores
  *(workload) = escr;
  *(workload+1) = mutex1;
  // prioridad escritores
  *(workload+2) = mutexcescr;
  *(workload+3) = slect;
  *(workload+4) = sescr;
  *(workload+5) = mutex2;
  *(workload+6) = mutextlect;
  *(workload+7) = mutexB;

  c->writeMemg(string(fileName),tamano[0],memg,litnum,limite[1],litstr,limite[2]);

  return 0;
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
  numArch=1;
  //cin>>numArch;
  string archivos[numArch];
  // for(int i=0; i<numArch; i++){
  //   cout<<"ingrese nombre del archivo: ";
  //   cin>>archivos[i];
  // }
  archivos[0]="test.bew";
  //archivos[1]="test.bew";
  Control* c = new Control();
  c->mainMemory(memoryName,fileName, c);
  Interpreter* interewe = new Interpreter(c->memg, c->litnum, c->litstr, c->datanum, c->datastr, c->workload);

  pid_t proc;
  for(int i=0; i<numArch; i++){
    if ((proc = ::fork()) == 0) {
      cout << "---------------------------proceso " << i << endl;
      interewe->proceso(archivos[i]);
      _exit(EXIT_SUCCESS);
    }
  }
  int status;
  waitpid(proc, &status, 0);
  //waitpid(proc2, &status, 0);
  return 0;
}
