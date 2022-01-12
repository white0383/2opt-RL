#include <iostream>
#include <vector>
#include <utility> // std::pair
#include <cstdlib> // abs(int num)
#include <float.h> // DBL_MAX
#include <map>

#include "FastTwoOpt.h"
#include "../../../helper/RandVec.h"
#include "../../../helper/mt19937ar.h"


using namespace std;

/* for debugging
#include <string>
#include <ctime>
void echo(string msg){
  cout << msg << endl;
}
void echo(vector<int> vec){
  for(auto foo:vec) cout << foo << " ";
  cout << endl;
}
*/

namespace FastTwoOptHelper{
  bool isAdjacent(int i, int j, int n){
    return (abs(i-j) == 1) || (abs(i-j) == n-1);
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

  double getAfterMinusBefore(Tour& pi, pair<int,int> IJ, const Graph& g){
    int i = IJ.first;
    int j = IJ.second;

    int p = pi.getPi(i);
    int pp = pi.getPi(i+1);
    int q = pi.getPi(j);
    int qp = pi.getPi(j+1);

    return getAfterMinusBefore(p,pp,q,qp,g);
  }


  void swapTwoOpt(Tour& pi,int i,int j){
    if(j == 0) j = pi.getSize(); // hold j > i

    if(i > j){ // hold j > i
      swapTwoOpt(pi,j,i);
    } else {
      do{
        pi.swap(i+1,j);
        i++;
        j--;
      }while((j - i) > 1);
    }
  }

  void swapTwoOpt(Tour& pi, pair<int,int> IJ){
    swapTwoOpt(pi,IJ.first,IJ.second);
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
} // end namespace FastTwoOptHelper

Tour fastTwoOpt(const Graph& g, Tour& pi){ 
  Tour pi_star = pi;
  int n = pi_star.getSize();
  bool improved = true;
  int swapNum = 0;

  while(improved){
    // reset "improved"
    improved = false;

    vector<int> i_vec = genRandIntVec(n);

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
        if(FastTwoOptHelper::isAdjacent(i,j,n)) continue; // for C

        amb = FastTwoOptHelper::getAfterMinusBefore(p,pp,q,qp,g);
        if(amb < 0){
          FastTwoOptHelper::swapTwoOpt(pi_star,i,j);
          swapNum++;
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
        if(FastTwoOptHelper::isAdjacent(i,j,n)) continue; // for Cp

        amb = FastTwoOptHelper::getAfterMinusBefore(p,pp,q,qp,g);
        if(amb < 0){
          FastTwoOptHelper::swapTwoOpt(pi_star,i,j);
          swapNum++;
          improved = true;
          break; // for Cp
        }
      } // end for Cp
      if(improved) {
        break; // for i
      }
    } // end for i
  } // end while

  pi_star.setSwapNum(swapNum);
  return pi_star;
}

Tour fastTwoOptBestInP(const Graph& g, Tour& pi, double pr){
  //Exception check
  if((pr<0) || (pr>1)){
    cout << "ERROR : fastTwoOptBestInP improper input p\n";
    cout << "p should be in [0,1]" << endl;
    cout << "yout p : " << pr << endl;
    exit(1); 
  }

  Tour pi_star = pi;
  int n = pi_star.getSize();
  bool improved = true;
  // after minus before
  // conduct 2optmove when amb < 0  
  double amb =0;
  map<double, pair<int,int> > scoreMap;
  int swapNum=0;

  while(improved){
    // reset "improved"
    improved = false;
    scoreMap.clear();

    vector<int> i_vec = genRandIntVec(n);

    int i,j,p,pp,q,qp;

    for(int i: i_vec){
      p = pi_star.getPi(i);
      pp = pi_star.getPi(i+1);

      for(int C = 1; C < n ; C++){
        q = g.distOrder[p][C];
        j = pi_star.getPiInv(q);
        qp = pi_star.getPi(j+1);

        if(q == pp) break; // for C
        if(FastTwoOptHelper::isAdjacent(i,j,n)) continue; // for C

        amb = FastTwoOptHelper::getAfterMinusBefore(p,pp,q,qp,g);
        if(amb < 0){
          scoreMap[-amb] = make_pair(i,j);
          improved = true;
        }
      } // end for C

      for(int Cp=1;Cp < n ; Cp++){
        qp = g.distOrder[pp][Cp];
        j = pi_star.getPiInv(qp) -1;
        q = pi_star.getPi(j);

        if(qp == p) break; // for Cp
        if(FastTwoOptHelper::isAdjacent(i,j,n)) continue; // for Cp

        amb = FastTwoOptHelper::getAfterMinusBefore(p,pp,q,qp,g);
        if(amb < 0){
          scoreMap[-amb] = make_pair(i,j);
          improved = true;
        }
      } // end for Cp
    } // end for i

    if(improved){
      FastTwoOptHelper::swapTwoOpt(pi_star, FastTwoOptHelper::getIJ(scoreMap,pr));
      swapNum++;
    }
  } // end while

  pi_star.setSwapNum(swapNum);
  return pi_star;
}


Tour fastTwoOptBest(const Graph& g, Tour& pi){
  return fastTwoOptBestInP(g,pi,1);
}
Tour fastTwoOptLeast(const Graph& g, Tour& pi){
  return fastTwoOptBestInP(g,pi,0);
}

Tour fastTwoOptMix(const Graph& g, Tour& pi, double pr){
  //Exception check
  if((pr<0) || (pr>1)){
    cout << "ERROR : fastTwoOptBestMix improper input p\n";
    cout << "p should be in [0,1]" << endl;
    cout << "yout p : " << pr << endl;
    exit(1); 
  }  

  Tour pi_star = pi;
  int n = pi_star.getSize();
  bool improved = true;

  bool isSkipped_firstImp = true;
  bool isFirstImp = true;
  map<double, pair<int,int> > scoreMap;
  int swapNum = 0;
  
  while(improved){
    improved = false;
    isSkipped_firstImp = false;

    vector<int> i_vec = genRandIntVec(n);
    int i,j,p,pp,q,qp;
    double amb=0;
    if(pr > genrand_real3()){
      isFirstImp = true;
    } else {
      isFirstImp = false;
    }
    scoreMap.clear();

    for(int i : i_vec){
      p = pi_star.getPi(i);
      pp = pi_star.getPi(i+1);

      for(int C = 1; C < n ; C++){
        q = g.distOrder[p][C];
        j = pi_star.getPiInv(q);
        qp = pi_star.getPi(j+1);

        if(q == pp) break; // for C
        if(FastTwoOptHelper::isAdjacent(i,j,n)) continue; // for C

        amb = FastTwoOptHelper::getAfterMinusBefore(p,pp,q,qp,g);
        if(amb < 0){
          improved = true;
          if(isFirstImp){ // first Improvement
            isSkipped_firstImp = true;
            swapNum++;
            FastTwoOptHelper::swapTwoOpt(pi_star,i,j);
            break;
          } else { // best Improvement
            scoreMap[-amb] = make_pair(i,j);
          }

        }
      } // end for C
      if(improved && isSkipped_firstImp){ // first Improvement
        break; // for i
      }

      for(int Cp=1;Cp < n ; Cp++){
        qp = g.distOrder[pp][Cp];
        j = pi_star.getPiInv(qp) -1;
        q = pi_star.getPi(j);

        if(qp == p) break; // for Cp
        if(FastTwoOptHelper::isAdjacent(i,j,n)) continue; // for Cp

        amb = FastTwoOptHelper::getAfterMinusBefore(p,pp,q,qp,g);
        if(amb < 0){
          improved = true;
          if(isFirstImp){ // first Improvement
            isSkipped_firstImp = true;
            swapNum++;
            FastTwoOptHelper::swapTwoOpt(pi_star,i,j);
            break;
          } else { // best Improvement
            scoreMap[-amb] = make_pair(i,j);
          }
        }
      } // end for Cp
      if(improved && isSkipped_firstImp){ // first Improvement
        break; // for i
      }
    } // end for i

    if(improved && !(isFirstImp)){ // best Improvement
      FastTwoOptHelper::swapTwoOpt(pi_star, FastTwoOptHelper::getMaxScoredIJ(scoreMap));
      swapNum++;
    }

  } // end while

  pi_star.setSwapNum(swapNum);
  return pi_star;
}

//========= Fast 2-opt big-j ========
/*
namespace FastTwoOptBigJHelper{
  bool isInvalid(int i, int j, int n){
    if(j <= i+1) return true;
    if((i==1) && (j==n)) return true;
    return false;
  }

  **
   * new tour will cut edge (p,pp) , (q,qp)
   * and reconnect them as (p,q) , (pp,qp)
   * 
   * when d(p,pp) + d(q,qp) > d(p,q) + d(pp,qp)
   * new tour is shorter than old tour
   * 
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

Tour fastTwoOptBigJ(const Graph& g, Tour& pi){
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
        if(FastTwoOptBigJHelper::isInvalid(i,j,n)) continue; // for C

        amb = FastTwoOptBigJHelper::getAfterMinusBefore(p,pp,q,qp,g);
        if(amb < 0){
          FastTwoOptBigJHelper::swapTwoOpt(pi_star,i,j);
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
        if(FastTwoOptBigJHelper::isInvalid(i,j,n)) continue; // for Cp

        amb = FastTwoOptBigJHelper::getAfterMinusBefore(p,pp,q,qp,g);
        if(amb < 0){
          FastTwoOptBigJHelper::swapTwoOpt(pi_star,i,j);
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
*/