#pragma once
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

using namespace std;
class Interpreter {

  int *memg;
  int *litnum;
  int *litstr;
  int *datanum;
  char *datastr;
  sem_t *workload;

 public:
  Interpreter(int *memg1, int *litnum1, int *litstr1, int *datanum1, char *datastr1, sem_t *workload1);
  ~Interpreter();
  void writeMemg(string nameFile,int limit, int *memg,int *litnum,int limiteNum,int *litstr,int limiteStr);
  int readMemg(int *memg, int pos);
  int readLitnum(int *litnum, int pos);
  char* readLitstr(int *litstr, int pos, int limite);
  char* readDatastr(char *datastr, int pos, int tamano);
  int readDatanum(int *datanum, int pos);
  void writeDatastr(char *datastr, int pos, char* data);
  void writeDatanum(int *datanum, int pos, int data);
  int interprete(int opcode, string value,int pc);
  void proceso(string file);
};
