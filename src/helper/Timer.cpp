#include "./Timer.h"

#include <ctime>
#include <iostream>

using namespace std;

void printTime(){
  time_t timer = time(NULL);
  struct tm* t;
  t = localtime(&timer);

  printf("%04d-%02d-%02d %02d:%02d:%02d\n",  
    t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,  
    t->tm_hour, t->tm_min, t->tm_sec);
}