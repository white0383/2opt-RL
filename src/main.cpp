#include <iostream>
#include <string>
#include <vector>
#include <map>

// includes
#include "./model/Arguments.h"
//#include "./RLmodel/LinearFittedQIteration.h"

using namespace std;

const map<string, double> OPT_VALUE {
  // TSPLIB
  {"eri51",426.0},
  {"berlin52",7542.0},
  {"st70",675.0},
  // VLSI
  {"xqf131", 564.0},
  {"xqg237", 1019.0},
  {"pma343", 1368.0},
  {"xql662", 2513.0}
};

int main(){
  /************************************/
  /**** Set your test config here. ****/
  /************************************/
  //**** Methods
  vector<string> TSPLIB_instances = {"eri51", "berlin52", "st70"};
  vector<string> VLSI_instances = {"xqf131", "xqg237", "pma343","xql662"};
  string tourInitMethod = "RT"; // RT FI
  string learnTermiCondi = "EPI"; // EPI SEC
  string thetaInitMethod = "UNIFM"; // UNI GAU ONE

  //**** Learning Hyperparameters
  unsigned int T = 100; // the number of samples for each learning
  unsigned int TMAX = 200; // the number of steps per theta update
  unsigned int MMAX = 600; // maximum size of MDP queue
  unsigned int LAMBDA = 5; // parameter in calculating rho
  unsigned int HMIN = 3; // minimum axis divisions in partition
  unsigned int HMAX = 10; // maximum axis divisions in partition
  double alpha = 1.1; // in reward of MDP
  double beta = 0.95; // in reward of MDP
  double gamma = 0.95; // discount factor
  double thetaInitPara = 2.0; 
  double greedyEps = 0.1; // probability of random action

  //**** Other Hyperparameters
  unsigned int SEED =  12080300;
  unsigned int EPILMT = 50;
  double secLmt = 180.0;

  /* use below while implementation */
  vector<string> argSTR = {TSPLIB_instances[0], tourInitMethod, learnTermiCondi, thetaInitMethod};
  vector<unsigned int> argINT = {T, TMAX, MMAX, LAMBDA,HMIN,HMAX, SEED,EPILMT};
  vector<double> argREA = {alpha, beta, gamma, thetaInitPara, greedyEps, secLmt};

  //Arguments tspArgs = Arguments(argSTR, argINT, argREA)
  //LinearFittedQIteration LinQ = LinearFittedQIteration(tspArgs);
  //LinQ.learn(tspArgs);

  /* use below in real experiments */
  // Nothing yet

  return 0;
}