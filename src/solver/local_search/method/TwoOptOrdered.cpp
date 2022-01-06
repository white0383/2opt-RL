#include <iostream>
#include <vector>
#include <cfloat> // DBL_MAX
#include <algorithm> // std::iter_swap
#include <map>
#include <utility> // std::pair

#include "TwoOptOrdered.h"
#include "../../../helper/RandVec.h"
#include "../../../helper/mt19937ar.h"

using namespace std;

namespace TwoOptOrderedHelper{
  /**
   * 2-opt is like below : (i) represent ith node on tour
   * 
   * ->  (i)  ->   <-  (j)  <-    swap     ->  (i)  ->  (j)  -> 
   *             ×                ===>
   * <- (j+1) <-   -> (i+1) ->             <- (j+1) <- (i+1) <-
   * 
   * dist(a,b) represent distance between ath node and bth node on tour.
   * Swap occurs when dist(i, i+1) + dist(j, j+1) > dist(i,j) + dist(i+1,j+1)
   * 
   * To make nodes name simple,
   * (i)   is node A
   * (i+1) is node B
   * (j)   is node C
   * (j+1) is node D
   * And
   * Swap when dist(A,B) + dist(C,D) > dist(A,C) + dist(B,D)
   */
  double getAfterMinusBefore(vector<int> pi_orders, const Graph& g, int i, int j){
    // ith element in original pi is equal to pi_orders[i-1]
    int A = pi_orders[(i)   -1];
    int B = pi_orders[(i+1) -1];
    int C = pi_orders[(j)   -1];
    int D = pi_orders[((j+1) -1) % pi_orders.size()];

    double distBeforeSwap = g.distMatrix[A][B] + g.distMatrix[C][D];
    double distAfterSwap = g.distMatrix[A][C] + g.distMatrix[B][D];
    
    return distAfterSwap - distBeforeSwap;
  }

  /**
   * "Swap" operator in 2-opt is like below (https://en.wikipedia.org/wiki/2-opt)
   * 
   *  input vector pi, int i and j
   *        (a) represent pi[a]
   *  1. take (start) to (i-1) and add them in order to new_pi
   *  2. take (i) to (j) and add them in reverse order to new_pi
   *  3. take (j+1) to (end) and add them in order to new_pi
   *  return new_pi
   */
  void swapTwoOpt(vector<int>& pi, int i, int j){
    vector<int>::iterator ite_head = pi.begin() + i;
    vector<int>::iterator ite_tail = pi.begin() + j - 1;

    do{
      iter_swap(ite_head,ite_tail);

      ite_head++;
      ite_tail--;
    }while(ite_tail - ite_head > 0);
  }

  void swapTwoOpt(vector<int>& pi, pair<int,int> IJ){
    swapTwoOpt(pi,IJ.first, IJ.second);
  }

  pair<int,int> getMaxScoredIJ(map<double,pair<int,int> >& m){
    return m.rbegin()->second;
  }

  pair<int,int> getMinScoredIJ(map<double,pair<int,int> >& m){
    return m.begin()->second;
  }

  pair<int,int> getIJ(map<double,pair<int,int> >& m, double p){
    if(p == 1){
      return getMaxScoredIJ(m);
    } else if(p == 0){
      return getMinScoredIJ(m);
    } 

    // then p is in (0,1)
    if(p > genrand_real3()){
      return getMaxScoredIJ(m);
    } else {
      return getMinScoredIJ(m);
    }

  }  
} // end namespace TwoOptOrderedHelper

Tour twoOptOrdered(const Graph& g, Tour& pi){
  vector<int> pi_orders = pi.getOrders();
  int n = pi.getSize();
  bool improved = true;
  double amb = 0;

  while(improved){
    improved = false;
    for(int i=1;i<= n-2;i++){
      for(int j=i+2;j<=n;j++){
        //exception handling. adjacent edge
        if((i == 1) && (j == n)) continue;

        amb = TwoOptOrderedHelper::getAfterMinusBefore(pi_orders,g,i,j);
        if(amb < 0){
          TwoOptOrderedHelper::swapTwoOpt(pi_orders, i, j);
          improved = true;
          break;
        } // end if
      } // end for j
      if(improved){ // break for i
        break;
      }
    } // end for i
  } // end while
  
  Tour pi_star(pi_orders,g);
  return pi_star;
}

Tour twoOptOrderedBestInP(const Graph& g, Tour& pi, double p){
  //Exception check
  if((p<0) || (p>1)){
    cout << "ERROR : twoOptBestOrderedInP improper input p\n";
    cout << "p should be in [0,1]" << endl;
    cout << "yout p : " << p << endl;
    exit(1); 
  }

  vector<int> pi_orders = pi.getOrders();
  int n = pi.getSize();
  bool improved = true;
  double amb = 0;
  map<double, pair<int,int> > scoreMap;

  while(improved){
    improved = false;
    scoreMap.clear();

    for(int i=1;i<= n-2;i++){
      for(int j=i+2;j<=n;j++){
        //exception handling. adjacent edge
        if((i == 1) && (j == n)) continue;

        amb = TwoOptOrderedHelper::getAfterMinusBefore(pi_orders,g,i,j);
        if(amb < 0){
          scoreMap[-amb] = make_pair(i,j);
          improved = true;
        } // end if
      } // end for j
    } // end for i

    if(improved){
      TwoOptOrderedHelper::swapTwoOpt(pi_orders,TwoOptOrderedHelper::getIJ(scoreMap,p));
    }
  } // end while
  
  Tour pi_star(pi_orders,g);
  return pi_star;
}

Tour twoOptOrderedBest(const Graph& g, Tour& pi){
  return twoOptOrderedBestInP(g,pi,1);
}
Tour twoOptOrderedLeast(const Graph& g, Tour& pi){
  return twoOptOrderedBestInP(g,pi,0);
}