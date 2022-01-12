#include "./LinearFittedQIteration.h"
#include "../helper/mt19937ar.h"
#include "../model/Arguments.h"
#include "../solver/initial_solution/GenerateInitialSolution.h"
#include "../solver/local_search/method/FastTwoOpt.h"
#include "../helper/RandVec.h"
#include "../helper/fitLP.h"

#include <ctime>
#include <vector>
#include <deque>
#include <cfloat> // DBL_MAX
#include <iostream>
#include <random> // mt19937, normal_distribution
#include <cmath> // pow

using namespace std;

bool DEBUG = false;

void ECHO(string msg){
  if(DEBUG) cout << msg << endl;
}

//========= LinQHelper ===============================================
namespace LinQHelper {

//===================== in LinQ Constructor =============================
  /**
   * Generate weight vecotr which has random double-type values
   * there values are included in [ (+-) tspArgs.THETA_INITPARA/2]
   */
  vector<double> genRandomTheta_Uniform(const Arguments& tspArgs){
    vector<double> rst_theta;
    rst_theta.reserve(tspArgs.K + 1);
    double theta =0.0;
    for(int i=0;i<tspArgs.K +1;i++){
      theta = (genrand_real1()-0.5 ) * tspArgs.THETA_INITPARA;
      rst_theta.push_back(theta);
    }
    return rst_theta;
  }

  /**
   * Generate weight vecotr which has gaussian double-type values
   * each of them are i.i.d of N(0,THETA_INITPARA)
   */
  vector<double> genRandomTheta_Gaussian(const Arguments& tspArgs){
    vector<double> rst_theta;
    rst_theta.reserve(tspArgs.K + 1);
    // set gaussian RNG
    mt19937 gen(genrand_int32());
    normal_distribution<double> gauss(0,tspArgs.THETA_INITPARA);
    double theta =0.0;
    for(int i=0;i<tspArgs.K +1;i++){
      theta = gauss(gen);
      rst_theta.push_back(theta);
    }
    return rst_theta;    
  }

  /**
   * Generate K+1 dimentional vector 
   * whose every member is THETA_INITPARA
   */
  vector<double> genRandomTheta_One(const Arguments& tspArgs){
    vector<double> rst_theta(tspArgs.K+1,tspArgs.THETA_INITPARA);

    return rst_theta;
  }

  vector<double> genRandomTheta(const Arguments& tspArgs){
    if(tspArgs.THETA_INIT_METHOD == "UNI"){
      return genRandomTheta_Uniform(tspArgs);
    } else if (tspArgs.THETA_INIT_METHOD == "GAU"){
      return genRandomTheta_Gaussian(tspArgs);
    } else if (tspArgs.THETA_INIT_METHOD == "ONE"){
      return genRandomTheta_One(tspArgs);
    }
  }; 

  deque<MDP> genInitReplayBuffer(){
    deque<MDP> rstDeq;
    return rstDeq;
  }

  int getEdgeIndexInPartition(int a, int b, int h){
    if( b < a ){
      return getEdgeIndexInPartition(b,a,h);
    }

    return (a-1)*(2*h*h - a)/2 + (b-1);
  }

} // end namespace LinQHelper

//========= LinQ Member ==============================================
LinearFittedQIteration::LinearFittedQIteration(const Arguments& tspArgs){
  this->theta = LinQHelper::genRandomTheta(tspArgs);
  this->replayBuffer = LinQHelper::genInitReplayBuffer();

  this->startTimeT = clock();
  this->spendSec = 0.0;
  this->MAXspendSec = tspArgs.SEC_LIMIT;

  this->time = 1;
  this->step = 1;
  this->MAXstep = tspArgs.TMAX;
  this->epi = 1;
  this->MAXepi = tspArgs.EPI_LIMIT;
  this->initTourCount = 0;
  this->swapCount = 0;

  LinearFittedQIteration::resetState(tspArgs);
  this->bestTour = this->s_now;

  this->tourLengthChange = 0;
  this->eventVec.clear();
}

void LinearFittedQIteration::learn(const Arguments& tspArgs){
  //cout << "===== Learning start =====" << endl;
  this->eventVec.emplace_back(Event("learn begin",*this));
  this->timeVec.assign(12,0);

  time_t startLearn = clock();
  time_t startMemo = 0;

  while(LinearFittedQIteration::checkTerminationCondition(tspArgs) == false){
    
    startMemo = clock();
    pair<int,int> a_now = this->searchAction(tspArgs); // search 2-opt neib.
    this->timeVec.at(1) += clock() - startMemo;

    startMemo = clock();
    double r_now = this->calcReward(tspArgs);
    this->timeVec.at(2) += clock() - startMemo;

    startMemo = clock();
    MDP mdp_now = MDP(a_now,r_now,*this);
    this->timeVec.at(3) += clock() - startMemo;

    startMemo = clock();
    this->update(a_now,mdp_now,tspArgs);
    this->timeVec.at(4) += clock() - startMemo;
    //cout << this->s_now.getCost() << endl;
  }

  this->timeVec.at(0) = clock() - startLearn;
  this->eventVec.emplace_back(Event("learn ends",*this));
  //cout << "Learning finish" << endl;
}

bool LinearFittedQIteration::checkTerminationCondition(const Arguments& tspArgs){
  if(tspArgs.LEARN_TERMINATE_METHOD == "EPI"){
    return (this->epi > this->MAXepi);
  }else if(tspArgs.LEARN_TERMINATE_METHOD == "SEC"){
    return (this->spendSec > this->MAXspendSec);
  }else if(tspArgs.LEARN_TERMINATE_METHOD == "TOUR"){
    return (this->initTourCount > this->MAXepi);
  }
}

pair<int,int> LinearFittedQIteration::searchAction(const Arguments& tspArgs){
  double pr = genrand_real3();
  pair<int,int> rst_actionIJ;
  if(pr < tspArgs.GREEDY_EPS){
    rst_actionIJ = this->searchActionRandom(tspArgs);
  } else {
    rst_actionIJ = this->searchActionGreedy(tspArgs);
  }

  if(rst_actionIJ.first > rst_actionIJ.second){ // when i > j
    int cp_i = rst_actionIJ.first;
    int cp_j = rst_actionIJ.second;
    rst_actionIJ = make_pair(cp_j, cp_i); // make first < second
  }
  
  this->setFeatureVector_action(rst_actionIJ,tspArgs);

  return rst_actionIJ;
}

// Like first improvement
pair<int,int> LinearFittedQIteration::searchActionRandom(const Arguments& tspArgs){
  int n = this->s_now.getSize();
  bool improved = false;
  double amb = 0;
  int i,j,p,pp,q,qp;
  pair<int,int> rst_IJ;

  while(improved == false){
    vector<int> i_vec = genRandIntVec(n);

    for(int i : i_vec){
      p = this->s_now.getPi(i);
      pp = this->s_now.getPi(i+1);

      for(int C = 1; C<n ; C++){
        q = tspArgs.V.distOrder[p][C];
        j = this->s_now.getPiInv(q);
        qp = this->s_now.getPi(j+1);

        if(q == pp) break; // for C
        if(FastTwoOptHelper::isAdjacent(i,j,n)) continue; // for C

        amb = FastTwoOptHelper::getAfterMinusBefore(p,pp,q,qp,tspArgs.V);
        if(amb < 0){
          rst_IJ = make_pair(i,j);
          improved = true;
          break; // for C
        }
      } // end for C
      if(improved){
        break; // for i
      }

      for(int Cp = 1;Cp<n;Cp++){
        qp = tspArgs.V.distOrder[pp][Cp];
        j = this->s_now.getPiInv(qp)-1;
        q = this->s_now.getPi(j);

        if(qp == p) break; // for Cp
        if(FastTwoOptHelper::isAdjacent(i,j,n)) continue; // for Cp

        amb = FastTwoOptHelper::getAfterMinusBefore(p,pp,q,qp,tspArgs.V);
        if(amb < 0){
          rst_IJ = make_pair(i,j);
          improved = true;
          break; // for Cp
        }
      } // end for Cp
      if(improved){
        break; // for i
      }
    } // end for i

    if(improved == false){
      this->resetState(tspArgs);
    }
  } // end while improved

  this->tourLengthChange = FastTwoOptHelper::getAfterMinusBefore(this->s_now,rst_IJ,tspArgs.V);
  return rst_IJ;
}

// Like best improvement
pair<int,int> LinearFittedQIteration::searchActionGreedy(const Arguments& tspArgs){
  int n = this->s_now.getSize();
  bool improved = false;
  double amb = 0;
  int i,j,p,pp,q,qp;
  pair<int,int> rst_IJ;  
  map<double, pair<int,int> > scoreMap;

  while(improved == false){
    scoreMap.clear();
    vector<int> i_vec = genRandIntVec(n);

    for(int i : i_vec){
      p = this->s_now.getPi(i);
      pp = this->s_now.getPi(i+1);

      for(int C = 1; C<n ; C++){
        q = tspArgs.V.distOrder[p][C];
        j = this->s_now.getPiInv(q);
        qp = this->s_now.getPi(j+1);

        if(q == pp) break; // for C
        if(FastTwoOptHelper::isAdjacent(i,j,n)) continue; // for C

        amb = FastTwoOptHelper::getAfterMinusBefore(p,pp,q,qp,tspArgs.V);
        if(amb < 0){
          scoreMap[LinearFittedQIteration::calcActionValue(p,pp,q,qp,tspArgs)] = make_pair(i,j);
          improved = true;
        }
      } // end for C

      for(int Cp = 1;Cp<n;Cp++){
        qp = tspArgs.V.distOrder[pp][Cp];
        j = this->s_now.getPiInv(qp)-1;
        q = this->s_now.getPi(j);

        if(qp == p) break; // for Cp
        if(FastTwoOptHelper::isAdjacent(i,j,n)) continue; // for Cp

        amb = FastTwoOptHelper::getAfterMinusBefore(p,pp,q,qp,tspArgs.V);
        if(amb < 0){
          scoreMap[LinearFittedQIteration::calcActionValue(p,pp,q,qp,tspArgs)] = make_pair(i,j);
          improved = true;
        }
      } // end for Cp
    } // end for i

    if(improved == false){
      this->resetState(tspArgs);
    }
  } // end while

  rst_IJ = FastTwoOptHelper::getMaxScoredIJ(scoreMap);
  this->tourLengthChange = FastTwoOptHelper::getAfterMinusBefore(this->s_now,rst_IJ,tspArgs.V);
  return rst_IJ;
}

void LinearFittedQIteration::resetState(const Arguments& tspArgs){
  this->eventVec.emplace_back(Event("local optima",*this));

  this->s_now = generateInitialSolution(tspArgs);
  this->initTourCount += 1;
  this->swapCount = 0;
  this->inNewRecord = false;
  s_now.setCost(tspArgs.V);
  LinearFittedQIteration::setFeatureVector_state(tspArgs);

  this->eventVec.emplace_back(Event("new initTour",*this));
}

void LinearFittedQIteration::setFeatureVector_state(const Arguments& tspArgs){
  this->featureVector_state.assign(tspArgs.K0,0);

  int n = this->s_now.getSize();
  double dist = 0;
  int p,pp;
  int partitionHBegin;
  int ithEdgeIndex;
  int pInPartitionH;
  int ppInPartitionH;

  for(int h = tspArgs.HMIN;h<= tspArgs.HMAX;h++){
    partitionHBegin = tspArgs.partitionBegins.at(h);
    const vector<int>& partitionH = tspArgs.partitions.at(h);
    for(int i=1;i<=n;i++){
      p = this->s_now.getPi(i);
      pp = this->s_now.getPi(i+1);
      dist = tspArgs.V.distMatrix.at(p).at(pp);

      pInPartitionH = partitionH.at(p);
      ppInPartitionH = partitionH.at(pp);

      ithEdgeIndex = partitionHBegin + LinQHelper::getEdgeIndexInPartition(pInPartitionH,ppInPartitionH,h);

      this->featureVector_state.at(ithEdgeIndex) += (dist / tspArgs.U);
    } // end for i
  } // end for h
}

void LinearFittedQIteration::setFeatureVector_action(pair<int,int> action, const Arguments& tspArgs){
  this->featureVector_action.clear();

  int i = action.first;
  int j = action.second;

  int p = this->s_now.getPi(i);
  int pp = this->s_now.getPi(i+1);
  int q = this->s_now.getPi(j);
  int qp = this->s_now.getPi(j+1);

  double d1 = tspArgs.V.distMatrix[p][pp]; //d(pi(i)  ,pi(i+1)) dropped
  double d2 = tspArgs.V.distMatrix[q][qp]; //d(pi(j)  ,pi(i+1)) dropped
  double d3 = tspArgs.V.distMatrix[p][q];  //d(pi(i)  ,pi(j)  ) added
  double d4 = tspArgs.V.distMatrix[pp][qp];//d(pi(i+1),pi(j+1)) added

  for(int h = tspArgs.HMIN ; h <= tspArgs.HMAX ; h++){
    int partitionHBegin = tspArgs.ActionBegin + tspArgs.partitionBegins.at(h);

    const vector<int>& partitionH = tspArgs.partitions.at(h);

    int pInPartitionH  = partitionH.at(p);
    int ppInPartitionH = partitionH.at(pp);
    int qInPartitionH  = partitionH.at(q);
    int qpInPartitionH = partitionH.at(qp);

    int d1Index = partitionHBegin + LinQHelper::getEdgeIndexInPartition(pInPartitionH,ppInPartitionH,h);
    int d2Index = partitionHBegin + LinQHelper::getEdgeIndexInPartition(qInPartitionH,qpInPartitionH,h);
    int d3Index = partitionHBegin + LinQHelper::getEdgeIndexInPartition(pInPartitionH,qInPartitionH,h);
    int d4Index = partitionHBegin + LinQHelper::getEdgeIndexInPartition(ppInPartitionH,qpInPartitionH,h);

    this->featureVector_action.emplace_back(make_pair(d1Index,-d1 / tspArgs.U));
    this->featureVector_action.emplace_back(make_pair(d2Index,-d2 / tspArgs.U));
    this->featureVector_action.emplace_back(make_pair(d3Index,d3 / tspArgs.U));
    this->featureVector_action.emplace_back(make_pair(d4Index,d4 / tspArgs.U));
  } // end for h
}

double LinearFittedQIteration::calcReward(const Arguments& tspArgs){
  double rst_reward = 0;

  rst_reward += pow(tspArgs.ALPHA, ((this->bestTour.getCost() * 1.20) / (this->s_now.getCost() + this->tourLengthChange))-1);
  rst_reward += tspArgs.BETA * ((this->s_now.getCost() / (this->s_now.getCost() + this->tourLengthChange)) -1);

  return rst_reward;
}

double LinearFittedQIteration::calcActionValue(int p,int pp, int q, int qp, const Arguments& tspArgs){
  double rst_actVal = 0;

  double d1 = tspArgs.V.distMatrix[p][pp]; //d(pi(i)  ,pi(i+1)) dropped
  double d2 = tspArgs.V.distMatrix[q][qp]; //d(pi(j)  ,pi(i+1)) dropped
  double d3 = tspArgs.V.distMatrix[p][q];  //d(pi(i)  ,pi(j)  ) added
  double d4 = tspArgs.V.distMatrix[pp][qp];//d(pi(i+1),pi(j+1)) added

  for(int h = tspArgs.HMIN ; h <= tspArgs.HMAX ; h++){
    int partitionHBegin = tspArgs.ActionBegin + tspArgs.partitionBegins.at(h);

    int pInPartitionH = tspArgs.partitions.at(h).at(p);
    int ppInPartitionH = tspArgs.partitions.at(h).at(pp);
    int qInPartitionH = tspArgs.partitions.at(h).at(q);
    int qpInPartitionH = tspArgs.partitions.at(h).at(qp);

    int d1Index = partitionHBegin + LinQHelper::getEdgeIndexInPartition(pInPartitionH,ppInPartitionH,h);
    int d2Index = partitionHBegin + LinQHelper::getEdgeIndexInPartition(qInPartitionH,qpInPartitionH,h);
    int d3Index = partitionHBegin + LinQHelper::getEdgeIndexInPartition(pInPartitionH,qInPartitionH,h);
    int d4Index = partitionHBegin + LinQHelper::getEdgeIndexInPartition(ppInPartitionH,qpInPartitionH,h);

      // original Q function (argmax) (converge to least improvement)
    //rst_actVal -= d1 * this->theta.at(d1Index) / tspArgs.U;
    //rst_actVal -= d2 * this->theta.at(d2Index) / tspArgs.U;
    //rst_actVal += d3 * this->theta.at(d3Index) / tspArgs.U;
    //rst_actVal += d4 * this->theta.at(d4Index) / tspArgs.U;

      // modified Q function (argmin) 
    rst_actVal += d1 * this->theta.at(d1Index) / tspArgs.U;
    rst_actVal += d2 * this->theta.at(d2Index) / tspArgs.U;
    rst_actVal -= d3 * this->theta.at(d3Index) / tspArgs.U;
    rst_actVal -= d4 * this->theta.at(d4Index) / tspArgs.U;
  }

  return rst_actVal;
}


void LinearFittedQIteration::update(pair<int,int>& a_now, MDP& mdp_now, const Arguments& tspArgs){
  time_t startMemo = clock();

  // update replayBuffer
  this->updateReplayBuffer(mdp_now, tspArgs);
  this->timeVec.at(5) += clock() - startMemo;

  // update s_now
  startMemo = clock();
  this->updateStateNow(a_now);
  this->timeVec.at(6) += clock() - startMemo;

  // update bestTime, bestDist
  startMemo = clock();
  if(this->bestTour.getCost() > this->s_now.getCost()){
    this->bestTour = this->s_now;
    
    if(this->inNewRecord == true) this->eventVec.pop_back(); // to prevent saving every info of the first initTour
    this->eventVec.emplace_back(Event("update best",*this));
    this->inNewRecord = true;

    //cout << "count "<< this->initTourCount <<" best " << this->bestTour.getCost() << endl;;
    this->updateBestScoreVec();
  }
  this->timeVec.at(7) += clock() - startMemo;

  // update fv
  startMemo = clock();
  this->updateFeatureVectorState();
  this->timeVec.at(8) += clock() - startMemo;

  // update time, step, spendSec
  this->time += 1;
  this->step += 1;
  this->spendSec = (double)(clock() - this->startTimeT) / CLOCKS_PER_SEC;
  
  // update theta when step > MAXstep
  if(this->step > this->MAXstep){
    this->eventVec.emplace_back(Event("epi end",*this));

    this->step = 1;
    this->epi += 1;
    startMemo = clock();
    this->updateTheta(tspArgs);
    this->timeVec.at(9) += clock() - startMemo;
    this->eventVec.emplace_back(Event("epi begin",*this));
  }
}

void LinearFittedQIteration::updateBestScoreVec(){
  if(this->bestScoreVec.empty()){
    this->bestScoreVec.emplace_back(make_pair(this->initTourCount, this->bestTour.getCost()));
    return;
  } else if(this->initTourCount == this->bestScoreVec.back().first){
    this->bestScoreVec.back().second = this->bestTour.getCost();
    return;
  } else {
    this->bestScoreVec.emplace_back(make_pair(this->initTourCount, this->bestTour.getCost()));
  }
}

void LinearFittedQIteration::updateReplayBuffer(MDP& mdp_now, const Arguments& tspArgs){
  this->replayBuffer.push_back(mdp_now);
  if(this->replayBuffer.size() > tspArgs.MMAX){
    this->replayBuffer.pop_front();
  }
}

void LinearFittedQIteration::updateStateNow(pair<int,int>& a_now){
  FastTwoOptHelper::swapTwoOpt(this->s_now,a_now);
  this->s_now.setCost(this->s_now.getCost() + this->tourLengthChange);
  this->swapCount += 1;
}

void LinearFittedQIteration::updateFeatureVectorState(){
  int K0 = this->featureVector_state.size();
  for(pair<int,double> actionFeature : this->featureVector_action){
    this->featureVector_state.at(actionFeature.first - K0) += actionFeature.second;
  }
}

void LinearFittedQIteration::updateTheta(const Arguments& tspArgs){
  // make DataSet
  time_t startMemo = clock();
  DataSet samples = DataSet(tspArgs, *this);
  this->timeVec.at(10) += clock() - startMemo;

  startMemo = clock();
  this->theta = fitLP(samples.featureVectors,samples.targetValues);
  this->timeVec.at(11) += clock() - startMemo;
}


void LinearFittedQIteration::printTimeVec(){
  cout << "  learn : " << (double)this->timeVec.at(0) / CLOCKS_PER_SEC << endl;
  cout << " action : " << (double)this->timeVec.at(1) / CLOCKS_PER_SEC << endl;
  cout << " reward : " << (double)this->timeVec.at(2) / CLOCKS_PER_SEC << endl;
  cout << "    mdp : " << (double)this->timeVec.at(3) / CLOCKS_PER_SEC << endl;
  cout << " update : " << (double)this->timeVec.at(4) / CLOCKS_PER_SEC << endl;
  cout << " RepBuf : " << (double)this->timeVec.at(5) / CLOCKS_PER_SEC << endl;
  cout << "  state : " << (double)this->timeVec.at(6) / CLOCKS_PER_SEC << endl;
  cout << "   best : " << (double)this->timeVec.at(7) / CLOCKS_PER_SEC << endl;
  cout << " feavec : " << (double)this->timeVec.at(8) / CLOCKS_PER_SEC << endl;
  cout << "  theta : " << (double)this->timeVec.at(9) / CLOCKS_PER_SEC << endl;
  cout << " datSet : " << (double)this->timeVec.at(10) / CLOCKS_PER_SEC << endl;  
  cout << "  cplex : " << (double)this->timeVec.at(11) / CLOCKS_PER_SEC << endl;
}

//========= DataSet Helper ========================================
namespace DataSetHelper{
  double calcStateValue(vector<double>& theta, vector<double>& stateFeatureVector){
    int K0 = stateFeatureVector.size();

    // check
    if(theta.size() == 2*K0 + 1) cout << "Theta size ok" << endl;

    double rst_stateValue = 0;
    for(int i=1;i<= K0;i++){
      rst_stateValue += theta.at(i) * stateFeatureVector.at(i-1);
    }

    return rst_stateValue;
  }

  double calcActionValue(int p, int pp, int q, int qp, const Arguments& tspArgs, vector<double>& theta){
    double rst_actVal = 0;

    double d1 = tspArgs.V.distMatrix[p][pp]; //d(pi(i)  ,pi(i+1)) dropped
    double d2 = tspArgs.V.distMatrix[q][qp]; //d(pi(j)  ,pi(i+1)) dropped
    double d3 = tspArgs.V.distMatrix[p][q];  //d(pi(i)  ,pi(j)  ) added
    double d4 = tspArgs.V.distMatrix[pp][qp];//d(pi(i+1),pi(j+1)) added

    for(int h = tspArgs.HMIN ; h <= tspArgs.HMAX ; h++){
      int partitionHBegin = tspArgs.ActionBegin + tspArgs.partitionBegins.at(h);

      int pInPartitionH = tspArgs.partitions.at(h).at(p);
      int ppInPartitionH = tspArgs.partitions.at(h).at(pp);
      int qInPartitionH = tspArgs.partitions.at(h).at(q);
      int qpInPartitionH = tspArgs.partitions.at(h).at(qp);

      int d1Index = partitionHBegin + LinQHelper::getEdgeIndexInPartition(pInPartitionH,ppInPartitionH,h);
      int d2Index = partitionHBegin + LinQHelper::getEdgeIndexInPartition(qInPartitionH,qpInPartitionH,h);
      int d3Index = partitionHBegin + LinQHelper::getEdgeIndexInPartition(pInPartitionH,qInPartitionH,h);
      int d4Index = partitionHBegin + LinQHelper::getEdgeIndexInPartition(ppInPartitionH,qpInPartitionH,h);

        // original Q function (argmax) (converge to least improvement)
      //rst_actVal -= d1 * theta.at(d1Index) / tspArgs.U;
      //rst_actVal -= d2 * theta.at(d2Index) / tspArgs.U;
      //rst_actVal += d3 * theta.at(d3Index) / tspArgs.U;
      //rst_actVal += d4 * theta.at(d4Index) / tspArgs.U;

        // modified Q function (argmin) 
      rst_actVal += d1 * theta.at(d1Index) / tspArgs.U;
      rst_actVal += d2 * theta.at(d2Index) / tspArgs.U;
      rst_actVal -= d3 * theta.at(d3Index) / tspArgs.U;
      rst_actVal -= d4 * theta.at(d4Index) / tspArgs.U;
    }

    return rst_actVal;
  }

  double calcMaxActionValue(vector<double>& theta, Tour& state, const Arguments& tspArgs){
    int n = state.getSize();
    double amb = 0;
    bool improved = false; // for check
    int i,j,p,pp,q,qp;
    double rst_maximumActionValue = -DBL_MAX;
    double tmp_actionValue = 0;

    while(improved == false){
      vector<int> i_vec = genRandIntVec(n);

      for(int i : i_vec){
        p = state.getPi(i);
        pp = state.getPi(i+1);

        for(int C = 1; C<n; C++){
          q = tspArgs.V.distOrder[p][C];
          j = state.getPiInv(q);
          qp = state.getPi(j+1);

          if(q == pp) break; // for C
          if(FastTwoOptHelper::isAdjacent(i,j,n)) continue; // for C

          amb = FastTwoOptHelper::getAfterMinusBefore(p,pp,q,qp,tspArgs.V);
          if(amb < 0){
            // Action score
            tmp_actionValue = DataSetHelper::calcActionValue(p,pp,q,qp,tspArgs,theta);
            if(tmp_actionValue > rst_maximumActionValue){
              rst_maximumActionValue = tmp_actionValue;
            }
            improved = true;
          }
        } // end for C

        for(int Cp = 1; Cp<n;Cp++){
          qp = tspArgs.V.distOrder[pp][Cp];
          j = state.getPiInv(qp)-1;
          q = state.getPi(j);

          if(qp == p) break; // for Cp
          if(FastTwoOptHelper::isAdjacent(i,j,n)) continue; // for Cp

          amb = FastTwoOptHelper::getAfterMinusBefore(p,pp,q,qp,tspArgs.V);
          if(amb < 0){
            tmp_actionValue = DataSetHelper::calcActionValue(p,pp,q,qp,tspArgs,theta);
            if(tmp_actionValue > rst_maximumActionValue){
              rst_maximumActionValue = tmp_actionValue;
            }            
            improved = true;
          }
        }// end for Cp
      } // end for i

    } // end while

    return rst_maximumActionValue;
  } // end calcMaxActionValue
}

//========= DataSet Member ===========================================
DataSet::DataSet(const Arguments& tspArgs, LinearFittedQIteration& LinQ){
  deque<MDP>& replayBuffer = LinQ.replayBuffer;

  this->sampleIndexs = genRandDiffIntVecBySet(0, replayBuffer.size()-1-tspArgs.LAMBDA, tspArgs.T);
  this->targetValues.reserve(tspArgs.T);
  this->featureVectors.reserve(tspArgs.T);

  for(int sampleIndex : this->sampleIndexs){
    double h = 0.0;
    double targetValue = 0.0;

    for(int j = 0; j<tspArgs.LAMBDA; j++){
      targetValue += pow(tspArgs.GAMMA,j) * replayBuffer.at(sampleIndex+j).reward;
    }

    // constant term of q function
    h += LinQ.theta.at(0);
    
    // calc state value of replayBuffer.at(sampleIndex+tspArgs.LAMBDA).state
    h += DataSetHelper::calcStateValue(LinQ.theta,replayBuffer.at(sampleIndex+tspArgs.LAMBDA).featureVector);

    // calc Maximum action value of replayBuffer.at(sampleIndex+tspArgs.LAMBDA).state
    h += DataSetHelper::calcMaxActionValue(LinQ.theta, replayBuffer.at(sampleIndex+tspArgs.LAMBDA).state, tspArgs);

    targetValue += pow(tspArgs.GAMMA,tspArgs.LAMBDA) * h;

    this->targetValues.emplace_back(targetValue);
    this->featureVectors.emplace_back(replayBuffer.at(sampleIndex).featureVector);
  }
}

void DataSet::printInfo(){
  cout << "======= Target Value :: ";
  for(auto foo : this->targetValues) cout << foo << " ";
  cout << endl << endl;

  cout << "======= Feature Matrix ::" << endl;
  int fv_count = 1;
  for(auto fv : this->featureVectors){
    //cout << "===FV " << fv_count++ << " : ";
    cout << "str_fv" << fv_count++ << " = '";
    for(auto f : fv) cout << f << " ";
    cout << "'"<< endl << endl;
  }
}

//========= MDP Member ===========================================
MDP::MDP(pair<int,int>& a, double r, LinearFittedQIteration& LinQ){
  this->time = LinQ.time;
  this->epi = LinQ.epi;
  this->step = LinQ.step;
  this->sec = LinQ.spendSec;

  this->state = LinQ.s_now;
  this->action = a;
  this->reward = r;

  decodeFeatureVector(LinQ);
}

void MDP::decodeFeatureVector(LinearFittedQIteration& LinQ){
  int K0 = LinQ.featureVector_state.size();
  int K = 2*K0;

  this->featureVector.assign(K,0);

  // set state part 0 ~ K0-1
  for(int i=0;i<K0;i++){
    this->featureVector.at(i) = LinQ.featureVector_state.at(i);
  }

  // set action part K0 ~ K-1
  for(pair<int,double> actionFeature : LinQ.featureVector_action){
    this->featureVector.at(actionFeature.first) += actionFeature.second;
  }
}

//========= Event Member ===========================================
Event::Event(char eventName[],LinearFittedQIteration& LinQ){
  this->name = eventName;
  this->epi  = LinQ.epi;
  this->step = LinQ.step;
  this->time = LinQ.time;
  this->tourCount = LinQ.initTourCount;
  this->swapCount = LinQ.swapCount;
  this->sec = (double)(clock() - LinQ.startTimeT) / CLOCKS_PER_SEC;
  this->bestScore = LinQ.bestTour.getCost();
  this->nowScore = LinQ.s_now.getCost();
}
void Event::print(ofstream& f){
  f << this->name << " ,";
  f << this->epi << " ,";
  f << this->step << " ,";
  f << this->time << " ,";
  f << this->tourCount << " ,";
  f << this->swapCount << " ,";
  f << this->nowScore << " ,";
  f << this->bestScore << " ,";
  f << this->sec << "\n";
}