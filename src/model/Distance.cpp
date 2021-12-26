#include <cmath>
#include "Distance.h"

double dist(const Node& n1,const Node& n2){
  double dx = n1.x - n2.x;
  double dy = n1.y - n2.y;
  
  return sqrt(dx*dx + dy*dy);
}

int distSquare(const Node& n1, const Node& n2){
  double dx = n1.x - n2.x;
  double dy = n1.y - n2.y;

  return dx * dx + dy * dy;
}
