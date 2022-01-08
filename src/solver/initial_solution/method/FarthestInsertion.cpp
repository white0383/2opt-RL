#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <float.h> // DBL_MAX

#include "FarthestInsertion.h"
#include "../../../helper/mt19937ar.h"
#include "../../../model/Tour.h"
#include "../../../model/Graph.h"
#include "../../../model/Distance.h"

using namespace std;

namespace FIHelper{
  // return min(v in pi) dist(u,v)
  double getDistTourNode(Tour& pi, int u, const Graph& g){
    // if u \in pi then dist == 0
    if(pi.getPiInv(u) != -1){
      return 0;
    }

    // Method 1
    /*
    double rst_minDist = DBL_MAX;
    double tmp_dist = 0;

    int tmp_i = 0;
    for(int i = 1;i<=pi.getSize();i++){
      tmp_dist = g.distMatrix[u][pi.getPi(i)];
      if(tmp_dist < rst_minDist){
        rst_minDist = tmp_dist;
        tmp_i = pi.getPi(i);
      }
    }

    return rst_minDist;
    */
    
    // Method 2 Win!! 
    double rst_minDist2 = DBL_MAX;
    int ithClosestCity = 0;
    for(int i=1;i<g.getN();i++){
      ithClosestCity = g.distOrder[u][i];
      if(pi.getPiInv(ithClosestCity) != -1){
        break;
      }
    }
    rst_minDist2 = g.distMatrix[u][ithClosestCity];
    //cout << "rst2: "<< ithClosestCity << " " << rst_minDist2 << endl;
    return rst_minDist2;
    
  }

  int findFarthestCity(Tour& pi, const Graph& g){
    int n = g.getN();
    int rst_k = 0;
    double tmpMaxDist = -DBL_MAX;
    double tmpDist = 0;
    for(int i=1;i<=n;i++){ // check every city in order
      if(pi.getPiInv(i) == -1){ // that is not included in pi
        tmpDist = FIHelper::getDistTourNode(pi,i,g);
        if(tmpDist > tmpMaxDist){
          tmpMaxDist = tmpDist;
          rst_k = i;
        }
      }
    }

    return rst_k;
  }

  int findWhereToInsert(Tour& pi, int k, const Graph& g){
    int rst_i = 0;
    double tmpMinDist = DBL_MAX;
    double tmpDist = 0;
    int pi_i = 0; // pi[i]
    int pi_ip = 0; // pi[i plus 1]

    for(int i=1; i <= pi.getSize(); i++){
      pi_i = pi.getPi(i);
      pi_ip = pi.getPi(i+1);
      tmpDist = g.distMatrix[pi_i][k] + g.distMatrix[k][pi_ip] - g.distMatrix[pi_i][pi_ip];

      if(tmpDist < tmpMinDist){
        tmpMinDist = tmpDist;
        rst_i = i;
      }
    }

    return rst_i;
  }
} // end namespace FIHelper

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
  int n = g.getN();

  // select random city index in [n] = {1,2, ... n-1, n}
  // the first city to visit
  // B is the farthest city from A.
  int cityAIndex = genrand_int32() % n + 1;
  int cityBIndex = g.distOrder[cityAIndex][n-1];

  //tmpOrder helps to make initTour
  //insert new city in tmpOrder with vector.insert method
  //and then re-construct initTour with Tour.setNewTour method
  vector<int> tmpOrder = {cityAIndex, cityBIndex};
  Tour initTour(tmpOrder, g);

  //Farthest City's index
  int fc=0;
  //Insertion City's index
  int ic=0;

  //While initTour visit every city,
  //add city fc between initTour.tour[ic] and initTour.tour[ic+1].
  //City fc is the FARTHEST(from initTour) city that is not included in initTour.
  //City ic is a city that is included in initTour, 
  //and it minimize d(tour[i],k) + d(k,pi[i+1]) - d(pi[i],pi[i+1]).
  //d(a,b) represent dist(g.nodes[a], g.nodes[b]).
  while(initTour.getSize() != n){
    fc = FIHelper::findFarthestCity(initTour, g);
    ic = FIHelper::findWhereToInsert(initTour, fc,g);
    //cout << "fc : " << fc << " ic : " << ic << endl;
    initTour.insert(fc,ic,g);
  }


  //double check initTour
  if(initTour.isCompleteTour(g)){
    return initTour;
  } else {
    cout << "ERROR : Initial Solution is not a complete tour" << endl;
    exit(1);
  }
}


Tour farthestInsertion(const Graph& g, mt19937& rng){
  int n = g.getN();

  // select random city index in [n] = {1,2, ... n-1, n}
  // the first city to visit
  // B is the farthest city from A.
  int cityAIndex = rng() % n + 1;
  int cityBIndex = g.distOrder[cityAIndex][n-1];

  //tmpOrder helps to make initTour
  //insert new city in tmpOrder with vector.insert method
  //and then re-construct initTour with Tour.setNewTour method
  vector<int> tmpOrder = {cityAIndex, cityBIndex};
  Tour initTour(tmpOrder, g);

  //Farthest City's index
  int fc=0;
  //Insertion City's index
  int ic=0;

  //While initTour visit every city,
  //add city fc between initTour.tour[ic] and initTour.tour[ic+1].
  //City fc is the FARTHEST(from initTour) city that is not included in initTour.
  //City ic is a city that is included in initTour, 
  //and it minimize d(tour[i],k) + d(k,pi[i+1]) - d(pi[i],pi[i+1]).
  //d(a,b) represent dist(g.nodes[a], g.nodes[b]).
  while(initTour.getSize() != n){
    fc = FIHelper::findFarthestCity(initTour, g);
    ic = FIHelper::findWhereToInsert(initTour, fc,g);
    //cout << "fc : " << fc << " ic : " << ic << endl;
    initTour.insert(fc,ic,g);
  }


  //double check initTour
  if(initTour.isCompleteTour(g)){
    return initTour;
  } else {
    cout << "ERROR : Initial Solution is not a complete tour" << endl;
    exit(1);
  }
}