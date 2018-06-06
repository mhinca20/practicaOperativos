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
class Control {

 public:
  Control();
  ~Control();
  void writeMemg(string nameFile,int limit, int *memg,int *litnum,int limiteNum,int *litstr,int limiteStr);
  int mainMemory(char* memoryName, string fileName);
  char* createMemory(int max, char* name);
  int *memg;
  int *litnum;
  int *litstr;
  int *datanum;
  char *datastr;
  sem_t *workload;
};
