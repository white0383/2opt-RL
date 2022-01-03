#include <iostream>
#include <vector>
#include <utility> // std::pair
#include <cstdlib> // abs(int num)
#include <float.h> // DBL_MAX

#include "tmp.h"
#include "../../../helper/RandVec.h"

using namespace std;

#include <string>
#include <ctime>
//void echo(string msg){
//  cout << msg << endl;
//}
//void echo(vector<int> vec){
//  for(auto foo:vec) cout << foo << " ";
//  cout << endl;
//}

namespace FastTwoOptTmpHelper{
  bool isInvalid(int i, int j, int n){
    if(j <= i+1) return true;
    if((i==1) && (j==n)) return true;
    return false;
  }

  /**
   * new tour will cut edge (p,pp) , (q,qp)
   * and reconnect them as (p,q) , (pp,qp)
   * 
   * when d(p,pp) + d(q,qp) > d(p,q) + d(pp,qp)
   * new tour is shorter than old tour
   */
  double getAfterMinusBefore(int p,int pp,int q,int qp, const Graph& g){
    double distBeforeSwap = g.distMatrix[p][pp] + g.distMatrix[q][qp];
    double distAfterSwap = g.distMatrix[p][q] + g.distMatrix[pp][qp];

    return distAfterSwap - distBeforeSwap;
  }

  void swapTwoOpt(Tour& pi,int i,int j){
    do{
      pi.swap(i+1,j);

      i++;
      j--;
    }while(j - i > 0);
  }
}

Tour fastTwoOptTmp(const Graph& g, Tour& pi){
  Tour pi_star = pi;
  int n = pi_star.getSize();
  bool improved = true;

  while(improved){
    // reset "improved"
    improved = false;

    vector<int> i_vec = genRandIntVec(n-2);

    int i,j,p,pp,q,qp;
    // after minus before
    // conduct 2optmove when amb < 0 
    double amb=0; 

    for(int i: i_vec){
      p = pi_star.getPi(i);
      pp = pi_star.getPi(i+1);

      for(int C = 1; C < n ; C++){
        q = g.distOrder[p][C];
        j = pi_star.getPiInv(q);
        qp = pi_star.getPi(j+1);

        if(q == pp) break; // for C
        if(FastTwoOptTmpHelper::isInvalid(i,j,n)) continue; // for C

        amb = FastTwoOptTmpHelper::getAfterMinusBefore(p,pp,q,qp,g);
        if(amb < 0){
          //cout << amb << " "<< i << ", "<<j<<endl;
          
          //cout << "before" << endl;
          //echo(pi_star.getPi());
          //echo(pi_star.getPiInv());
          FastTwoOptTmpHelper::swapTwoOpt(pi_star,i,j);
          //cout << "after" << endl;
          //echo(pi_star.getPi());
          //echo(pi_star.getPiInv());
          improved = true;
          break; // for C
        }
      } // end for C
      if (improved) {
        break; // for i
      }

      for(int Cp=1;Cp < n ; Cp++){
        qp = g.distOrder[pp][Cp];
        j = pi_star.getPiInv(qp) -1;
        q = pi_star.getPi(j);

        if(qp == p) break; // for Cp
        if(FastTwoOptTmpHelper::isInvalid(i,j,n)) continue; // for Cp

        amb = FastTwoOptTmpHelper::getAfterMinusBefore(p,pp,q,qp,g);
        if(amb < 0){
          //cout << amb << " "<< i << ", "<<j<<endl;
          
          //cout << "before" << endl;
          //echo(pi_star.getPi());
          //echo(pi_star.getPiInv());
          FastTwoOptTmpHelper::swapTwoOpt(pi_star,i,j);
          //cout << "after" << endl;
          //echo(pi_star.getPi());
          //echo(pi_star.getPiInv());
          improved = true;
          break; // for Cp
        }
      } // end for Cp
      if(improved) {
        break; // for i
      }
    } // end for i
  }

  return pi_star;
}