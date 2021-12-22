#include <cstdlib> // should be replaced to <random>'s mt19937
#include <iostream>
#include <vector>
#include <algorithm>
#include <random> // should be replaced to <random>'s mt19937

#include "RandomTour.h"
#include "../../../helper/mt19937ar.h"
#include "../../../model/Tour.h"
#include "../../../model/Graph.h"

using namespace std;

Tour randomTour(const Graph& g){
  mt19937 mtRand(genrand_int32()); 
  int cityNum = g.getN();

  // visitOrder is {1,2, ... ,cityNum-1, cityNum}
  vector<int> visitOrder(cityNum);
  for(int i=0;i<cityNum;i++){
    visitOrder[i] = i+1; // +1 : index of g.nodes + 1 = index of Node 
  }

  shuffle(visitOrder.begin(), visitOrder.end(), mtRand);

  Tour initTour(visitOrder, g);

  if(initTour.isCompleteTour(g)){
    return initTour;
  } else {
    cout << "ERROR : Initial Solution is not a complete tour" << endl;
    exit(1);
  }
}