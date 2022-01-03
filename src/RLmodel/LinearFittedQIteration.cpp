#include "LinearFittedQIteration.h"
#include "../helper/mt19937ar.h"
#include "../model/Arguments.h"
#include "../solver/initial_solution/GenerateInitialSolution.h"
#include "../helper/RandVec.h"
#include "../helper/fitLP.h"

#include <ctime>
#include <vector>
#include <deque>
#include <cfloat> // DBL_MAX
#include <iostream>
#include <random> // mt19937, normal_distribution

using namespace std;

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

    // statements below are just for c++ grammar
    // Only when Acguments' initialization has problem, 
    // below statement will run.
    vector<double> rst_dummy;
    cout << "ERROR : reinLearnMemoryHelper::genRandomWeights invalid Arguments' WEIGHT_INIT_METHOD" << endl;
    return rst_dummy;   
  }; 

  deque<MDP> genInitReplayBuffer(){
    deque<MDP> rstDeq;
    return rstDeq;
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

  this->s_now = generateInitialSolution(tspArgs);
  s_now.setCost(tspArgs.V);
  this->bestTour = this->s_now;
}

void LinearFittedQIteration::learn(const Arguments& tspArgs){
  //cout << "===== Learning start =====" << endl;

  while(LinearFittedQIteration::checkTerminationCondition(tspArgs) == false){
    //pair<int,int> a_now = LinQHelper::searchAction(s_now, *this, tspArgs);
    pair<int,int> a_now = this->searchAction(tspArgs); // search 2-opt neib.
    Tour s_next = this->transit(a_now,tspArgs); // 2-opt-move
    double r_now = this->calcReward(s_next,tspArgs);
    vector<double> f_now = this->calcFeatureVector(a_now,tspArgs);
    MDP mdp_now = MDP(a_now,r_now,f_now,*this);
    this->update(/* something */);
  }

  //cout << "Learning finish" << endl;
}

bool LinearFittedQIteration::checkTerminationCondition(const Arguments& tspArgs){
  if(tspArgs.TERMINATE_METHOD == "EPI"){
    return (this->epi > this->MAXepi);
  }else if(tspArgs.TERMINATE_METHOD == "SEC"){
    return (this->spendSec > this->MAXspendSec);
  }

  // statements below are just for c++ grammar
  // Only when Acguments' initialization has problem, 
  // below statement will run.
  cout << "Error : ReinLearnMemory::checkTerminationCondition invalid termination condition " << endl;
  cout << "Your input TERMINATE_METHOD is \"" << tspArgs.TERMINATE_METHOD <<"\""<< endl;
  exit(1);    
}

void LinearFittedQIteration::updateInfo(MDP& mdp, State& s_prev, State& s_next, const Arguments& tspArgs){
  // update replayBuffer
  this->updateReplayBuffer(mdp, tspArgs);

  // update lastTimeNodeActioned
  this->updateLastTimeNodeActioned(mdp.a_prev);

  // update bestTime, bestDist
  this->updateBestInfos(s_prev);

  // update distQueue
  //updateDistQueue(double dist_piStar_next, const Arguments& tstArgs){
  this->updateDistQueue(s_next.distPiStar, tspArgs);

  // s_next is next s_prev
  s_prev = s_next;

  // time inscrease
  this->time++;
  this->spendSec = (double)(clock() - this->startTimeT) / CLOCKS_PER_SEC;
}

void LinearFittedQIteration::updateReplayBuffer(MDP& prevMDP,const Arguments& tspArgs){
  this->replayBuffer.push_back(prevMDP);
  if(this->replayBuffer.size() > tspArgs.MMAX){
    this->replayBuffer.pop_front();
  }
}

void LinearFittedQIteration::updateLastTimeNodeActioned(Action& a_prev){
  for(pair<int,int> swap : a_prev.getSwaps()){
    this->lastTimeNodeActioned.at(swap.first) = this->time;
    this->lastTimeNodeActioned.at(swap.second) = this->time;
  }
}

void LinearFittedQIteration::updateBestInfos(State& s_prev){
  double dist_piStar_prev = s_prev.distPiStar;

  if(dist_piStar_prev < this->bestDist){
    //cout << "UPDATE! : time " << this->time << " " << this-> bestDist << " -> " << dist_piStar_prev << endl;
    this->bestDist = dist_piStar_prev;
    this->bestTime = this->time;
    this->bestTour = s_prev.pi_star;
  }
}

void LinearFittedQIteration::updateWeights(DataSet& dataSet){
  //this->weights = fitLinearQ(dataSet.featureVectors, dataSet.targetValues);
  this->weights = fitLP(dataSet.featureVectors, dataSet.targetValues);
}

void LinearFittedQIteration::printWeights(){
  cout << "EPI : " << this->epi;
  cout << "dim : " << this->weights.size() << endl;
  //cout << "Weights : ";
  //for(auto foo : this->weights) cout << foo << " ";
  //cout << endl;
  cout << "None zero :" << endl;
  for(int i = 0; i < this->weights.size() ; i++){
    if(this->weights.at(i) != 0){
      cout << "i : " <<  i << " value : " << this->weights.at(i) << endl;
    }
  }
}

//========= DataSet Member ===========================================
DataSet::DataSet(const Arguments& tspArgs, LinearFittedQIteration& LinQ){
  deque<MDP>& replayBuffer = LinQ.replayBuffer;

  this->sampleIndexs = genRandDiffIntVecBySet(0, replayBuffer.size()-1, tspArgs.T);
  this->targetValues.reserve(tspArgs.T);
  this->featureVectors.reserve(tspArgs.T);

  double h = 0.0;
  double targetValue = 0.0;

  for(int sampleIndex : sampleIndexs){
    Action a_sampled = Action(replayBuffer.at(sampleIndex).s_next, LinQ, tspArgs);
    h = MDPHelper::Qfunction(replayBuffer.at(sampleIndex).s_next, a_sampled, LinQ, tspArgs);

    targetValue = replayBuffer.at(sampleIndex).r_prev + tspArgs.GAMMA * h;
    vector<double> f_sampled = replayBuffer.at(sampleIndex).f_prev;

    this->targetValues.emplace_back(targetValue);
    this->featureVectors.emplace_back(f_sampled);
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