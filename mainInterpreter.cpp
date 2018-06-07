#include <pthread.h>
#include <semaphore.h>
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
#include "interpreter.h"

using namespace std;
int main(int argc, char* argv[]){

  if (argc != 4) {
    cerr << "Usage: "<< argv[0] << " -n <nommemcom> <ficherobinewe>" << endl;
    return 1;
  }
  char* memoryName=argv[2];
  string fileName=argv[3];

  int shm = shm_open(memoryName, O_RDWR , 0600);

  off_t size_mem =2147483647;
  char *pMem = static_cast<char*>(mmap(NULL, size_mem, PROT_READ | PROT_WRITE,MAP_SHARED, shm, 0));
  int *pInt = (int *)pMem;
  int base[6];

  for (int i = 0; i < 6; ++i) {
    base[i]= ((*(pInt + i))>>16);
  }

  int* memg = (int *)pMem;
  int* litnum = (int *)pMem+base[1];
  int* litstr = (int *)pMem+base[2];
  int* datanum = (int *)pMem+base[3];
  char* datastr = (char*)((int *)pMem+base[4]);
  sem_t* workload = (sem_t*)((int *)pMem+base[5]);


  Interpreter* interewe = new Interpreter(memg, litnum, litstr, datanum, datastr, workload);
  interewe->proceso(fileName);
  return 0;
}
