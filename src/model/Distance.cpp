#include <cmath>
#include <float.h>
#include "Distance.h"

/*
double dist(Node& n1, Node& n2){
  int dx = n1.x - n2.x;
  int dy = n1.y - n2.y;
  
  return sqrt((double)(dx*dx + dy*dy));
}
*/

double dist(const Node& n1,const Node& n2){
  int dx = n1.x - n2.x;
  int dy = n1.y - n2.y;
  
  return sqrt((double)(dx*dx + dy*dy));
}

double dist(const ScaledNode& sn1, const ScaledNode& sn2){
  double dx = sn1.x - sn2.x;
  double dy = sn1.y - sn2.y;

  return sqrt((double)(dx*dx + dy*dy));
}

int distSquare(const Node& n1, const Node& n2){
  int dx = n1.x - n2.x;
  int dy = n1.y - n2.y;

  return dx * dx + dy * dy;
}
