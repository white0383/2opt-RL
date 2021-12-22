#include <iostream>
#include <vector>
#include <utility> // std::pair
#include <cstdlib> // abs(int num)
#include <numeric> // std::iota
#include <random> // std::mt19937
#include <algorithm> // std::shuffle
#include <cfloat> // DBL_MAX

#include "ScaledFastTwoOpt.h"
#include "../../../model/Distance.h"
#include "../../../model/Node.h"
#include "../../../model/Graph.h"
#include "../../../helper/mt19937ar.h"

using namespace std;

void swapScaledFastTwoOptIn(vector<int>& pi, vector<int>& pi_inv, int i, int j){
  vector<int> pi_rev_tmp(j-i);

  //copy from pi[i+1] to pi[j] in reverse
  for(int it=0;it<j-i;it++){
    pi_rev_tmp[it] = pi[-it + j];
  }

  //reverse from pi[i+1] to pi[j]
  int pi_tmp_index=0;
  for(int it=0;it<j-i;it++){
    pi_tmp_index = it+i+1;
    pi[pi_tmp_index] = pi_rev_tmp[it];
    pi_inv[pi[pi_tmp_index]] = pi_tmp_index;
  }
}
    
void swapScaledFastTwoOpt(vector<int>& pi, vector<int>& pi_inv, int i, int j){
  if(i > j){
    swapScaledFastTwoOpt(pi, pi_inv, j, i);
  } else {
    swapScaledFastTwoOptIn(pi, pi_inv, i, j);
  }
}

//scaledFastTwoOpt(const Arguments& tspArgs, Tour& pi);
Tour scaledFastTwoOpt(const Arguments& tspArgs, Tour& pi){
  const Graph& g = tspArgs.V;
  int cityNum = pi.getSize();
  bool improved = true;
  double eps = tspArgs.EPS_SF2OPT;
  
  if(pi.getScaledCost() == DBL_MAX){
    cout << "WARNING : SF2OPT pi.scost is not calculated" << endl;
    pi.setScaledCost(tspArgs.V);
  }
  double distPiStar = pi.getScaledCost();

  vector<int> pi_vec = pi.getTour();
  pi_vec.insert(pi_vec.begin(),0);

  vector<int> pi_vec_inv(cityNum+1);
  for(int i=1;i<cityNum+1;i++){
    pi_vec_inv[pi_vec[i]] = i;
  }

  // i_vec initialized -> {1,2, ... , n-1, n}
  vector<int> i_vec(cityNum);
  iota(i_vec.begin(), i_vec.end(), 1);

  // *_order : 巡回路の何番目か
  // *_index : 都市の番号, 名前
  // *_node  : ノードそのもの
  while(improved){
    // reset "improved"
    improved = false;

    // shuffle "i_vec"
    mt19937 mtRand(genrand_int32());
    shuffle(i_vec.begin(), i_vec.end(),mtRand);

    for(int i_order : i_vec){
      ScaledNode p_snode = g.scaledNodes.at(pi_vec[i_order]);
      ScaledNode pp_snode;

      // Exception handling
      if(i_order != cityNum){
        pp_snode = g.scaledNodes.at(pi_vec[i_order+1]);
      } else {
        pp_snode = g.scaledNodes.at(pi_vec[1]);
      }

      for(int iC = 1; iC < cityNum ; iC++){
        int j_order = pi_vec_inv.at(g.distOrder[p_snode.index][iC].first);
        ScaledNode q_snode = g.scaledNodes.at(pi_vec[j_order]);
        ScaledNode qp_snode;

        // Exception handling
        if(j_order != cityNum){
          qp_snode = g.scaledNodes.at(pi_vec[j_order+1]);
        } else {
          qp_snode = g.scaledNodes.at(pi_vec[1]);
        }

        if (abs(i_order - j_order) > 1){
          double dist_p_q = dist(p_snode,q_snode);
          double dist_p_pp = dist(p_snode, pp_snode);
          if (dist_p_q < dist_p_pp) {
            double dist_pp_qp = dist(pp_snode, qp_snode);
            double dist_q_qp = dist(q_snode, qp_snode);
            double vari = (dist_p_pp + dist_q_qp) - (dist_p_q + dist_pp_qp);
            if(vari > eps){
              swapScaledFastTwoOpt(pi_vec, pi_vec_inv,i_order, j_order);
              //cout << "vari : " << vari << endl;
              distPiStar -= vari;
              improved = true;
              break;
            }
          } else {
            break;
          }
        }
      }
      if (improved) {
        break;
      }

      for(int iCp=1;iCp < cityNum ; iCp++){
        int jp_order = pi_vec_inv.at(g.distOrder[pp_snode.index][iCp].first);
        int j_order =jp_order - 1;
        if(j_order == 0) j_order = cityNum; // exception check
        ScaledNode qp_snode = g.scaledNodes.at(pi_vec[jp_order]);
        ScaledNode q_snode = g.scaledNodes.at(pi_vec[j_order]);

        if (abs(i_order - j_order) > 1){
          double dist_pp_qp = dist(pp_snode, qp_snode);
          double dist_p_pp = dist(p_snode, pp_snode);
          if (dist_pp_qp < dist_p_pp) {
            double dist_p_q = dist(p_snode, q_snode);
            double dist_q_qp = dist(q_snode, qp_snode);
            double vari = (dist_p_pp + dist_q_qp) - (dist_p_q + dist_pp_qp);
            if(vari > eps){
              swapScaledFastTwoOpt(pi_vec, pi_vec_inv,i_order, j_order);
              //cout << "vari : " << vari << endl;
              distPiStar -= vari;
              improved = true;
              break;
            }
          } else {
            break;
          }
        }
      }
      if (improved){
        break;
      }
    }
  }
  //cut head of pi_vec (0-th member)
  pi_vec.erase(pi_vec.begin());
  Tour pi_star(pi_vec,g);
  pi_star.setThisIsLocalOpt();
  pi_star.setScaledCost(distPiStar);

  return pi_star;
}