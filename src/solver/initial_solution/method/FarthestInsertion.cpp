#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <float.h>

#include "FarthestInsertion.h"
#include "../../../helper/mt19937ar.h"
#include "../../../model/Tour.h"
#include "../../../model/Graph.h"
#include "../../../model/Distance.h"

using namespace std;

int findFarthestCity(Tour& pi, const Graph& g, vector<bool>& included){
  int cityNum = g.getN();
  int rst_k = 0;
  double tmpMaxDist = 0;
  double tmpDist = DBL_MAX;
  for(int i=1;i<=cityNum;i++){ // check every city in order
    if(included[i] == false){ // that is not included in pi
      tmpDist = pi.getDistToNode(g.nodes[i],g); 
      if(tmpDist > tmpMaxDist){
        tmpMaxDist = tmpDist;
        rst_k = i;
      }
    }
  }

  return rst_k;
}

int findWhereToInsert(Tour& pi, const Graph& g, int k){
  int rst_i = 0;
  double tmpMinDist = DBL_MAX;
  double tmpDist = 0;
  Node cityK = g.nodes[k];
  for(int city=1; city <= pi.getSize(); city++){
    Node tmpCityI = pi.pi_node(city,g);
    Node tmpCityIplus = pi.pi_node(city+1,g);
    tmpDist =  dist(tmpCityI,cityK) + dist(cityK,tmpCityIplus) - dist(tmpCityI,tmpCityIplus);

    if(tmpDist < tmpMinDist){
      tmpMinDist = tmpDist;
      rst_i = city;
    }
  }

  return rst_i;
}

void insertNewCity(Tour& pi, const Graph& g, int k, int i, vector<int>& order, vector<bool>& included){
  order.insert(order.begin()+i,k);
  pi.setNewTour(order,g);
  if(included[k] == false){
    included[k] = true;
  } else {
    cout << "ERROR : insertNewCity : k in pi is not allowed" << endl;
    exit(1);
  }
}

Tour farthestInsertion(const Graph& g){
  int cityNum = g.getN();

  // select random city index in [cityNum] = {1,2, ... cityNum-1, cityNum}
  // the first city to visit
  // B is the farthest city from A.
  int cityAIndex = genrand_int32() % cityNum + 1;
  int cityBIndex = g.distOrder[cityAIndex][cityNum-1].first;

  //tmpOrder helps to make initTour
  //insert new city in tmpOrder with vector.insert method
  //and then re-construct initTour with Tour.setNewTour method
  vector<int> tmpOrder = {cityAIndex, cityBIndex};
  Tour initTour(tmpOrder, g);

  //Farthest City's index
  int fc=0;
  //Insertion City's index
  int ic=0;

  //This helps to check arbitrary Node's index is included in initTour.tour
  //Or to check arbitrary city is included or not.
  //every city except A and B is not included now
  //if included[i] is true, i is included in initTour.tour vector
  //Tour.isNodeIncluded can do same thing, but it takes O(n) in worst case.
  vector<bool> included(cityNum+1, false);
  included[cityAIndex] = true;
  included[cityBIndex] = true;

  //While initTour visit every city,
  //add city fc between initTour.tour[ic] and initTour.tour[ic+1].
  //City fc is the FARTHEST(from initTour) city that is not included in initTour.
  //City ic is a city that is included in initTour, 
  //and it minimize d(tour[i],k) + d(k,pi[i+1]) - d(pi[i],pi[i+1]).
  //d(a,b) represent dist(g.nodes[a], g.nodes[b]).
  while(initTour.getSize() != cityNum){
    fc = findFarthestCity(initTour, g, included);
    ic = findWhereToInsert(initTour, g, fc);
    insertNewCity(initTour, g, fc ,ic ,tmpOrder, included);
  }

  //double check initTour
  if(initTour.isCompleteTour(g)){
    return initTour;
  } else {
    cout << "ERROR : Initial Solution is not a complete tour" << endl;
    exit(1);
  }
}
