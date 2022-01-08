#include <iostream>
#include <string>
#include <vector>
#include <map>

// includes MUST
#include "./model/Arguments.h"
//#include "./RLmodel/LinearFittedQIteration.h"

// includes experiment
#include <ctime>
#include "./model/Tour.h"
#include "./solver/initial_solution/GenerateInitialSolution.h"
#include "./solver/local_search/SearchLocalOpt.h"
#include "float.h" // DBL_MAX
#include "./helper/mt19937ar.h"
#include "./helper/Timer.h"
#include <fstream>
//#include "./solver/local_search/method/TwoOpt.h"
//#include "./solver/local_search/method/TwoOptOrdered.h"
//#include "./solver/local_search/method/FastTwoOpt.h"
#include <random> // Tour rng

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

vector<string> WHOLE_FILES = {
"burma14", "ulysses16", "ulysses22", "att48", "eil51", "berlin52", "st70", "eil76", "pr76", "gr96", 
"rat99", "rd100", "kroA100", "kroB100", "kroC100", "kroD100", "kroE100", "eil101", "lin105", "pr107", 
"pr124", "bier127", "ch130", "xqf131", "pr136", "gr137", "pr144", "kroA150", "ch150", "kroB150", 
"pr152", "u159", "rat195", "d198", "kroA200", "kroB200", "gr202", "ts225", "tsp225", "pr226", 
"gr229", "xqg237", "gil262", "pr264", "a280", "pr299", "lin318", "linhp318", "pma343", "pka379", 
"rd400", "fl417", "pbn423", "gr431", "pr439", "pcb442", "d493", "att532", "ali535", "u574", 
"rat575", "p654", "d657", "xql662", "u724", "rat783", "pbd984", "dsj1000", "pr1002", "u1060", 
"vm1084", "pcb1173", "d1291", "rl1304", "rl1323", "dka1376", "nrw1379", "fl1400", "u1432", "fl1577", 
"d1655", "vm1748", "u1817", "rl1889", "d2103", "u2152", "u2319", "dea2382", "pr2392", "bch2762", 
"lsm2854", "dbj2924", "pcb3038", "pia3056", "dke3097", "lsn3119", "lta3140", "fdp3256", "beg3293", "dhb3386", 
"fjs3649", "fjr3672", "dlb3694", "ltb3729", "fl3795", "xqe3891", "xua3937", "dkc3938", "dkf3954", "fnl4461", 
"rl5915", "rl5934", "xsc6880", "pla7397", "lap7454", "ida8197", "dga9698", "xmc10150", "rl11849", "usa13509", 
"brd14051", "d15112", "d18512", "pla33810", "pla85900", 
};

int main(){
  /************************************/
  /**** Set your test config here. ****/
  /************************************/
  //**** Methods
  vector<string> TSPLIB_instances = {"eil51", "rd100", "pr152", "kroA200", "pr299"};
  vector<string> VLSI_instances = {"xqf131", "xqg237", "pma343","xql662"};
  string tourInitMethod = "FI"; // RT FI
  string learnTermiCondi = "SEC"; // EPI SEC
  string thetaInitMethod = "ONE"; // UNI GAU ONE

  //**** Learning Hyperparameters
  int T = 1200; // the number of samples for each learning
  int TMAX = 3600; // the number of steps per theta update
  int MMAX = 6000; // maximum size of MDP queue
  int LAMBDA = 5; // parameter in calculating rho
  int HMIN = 3; // minimum axis divisions in partition
  int HMAX = 5; // maximum axis divisions in partition
  double alpha = 3.0; // in reward of MDP
  double beta = 0.95; // in reward of MDP
  double gamma = 0.95; // discount factor
  double thetaInitPara = -1.0; 
  double greedyEps = 0.1; // probability of random action

  //**** Other Hyperparameters
  int SEED = 1071556; // Jan 7th 01:23
  int EPILMT = 50;
  double secLmt = 60.0;

  /*********************************/
  /***** Experiment code below *****/
  /*********************************/
    vector<string> argSTR = { TSPLIB_instances[0], tourInitMethod, learnTermiCondi, thetaInitMethod};
    vector<int> argINT = {T, TMAX, MMAX, LAMBDA,HMIN,HMAX, SEED,EPILMT};
    vector<double> argREA = {alpha, beta, gamma, thetaInitPara, greedyEps, secLmt};
    Arguments tspArgs = Arguments(argSTR,argINT,argREA);

    //LinQ.learn(tspArgs);
    //cout << "2opt-RL : best -> " << LinQ.bestTour.getCost() << endl;

    //========== RT,FI with rng test ===========
    int rngSEED = 1081419;
    mt19937 rng(rngSEED);
    Tour a0 = generateInitialSolution(tspArgs.V,tourInitMethod,rng);
    Tour a1 = generateInitialSolution(tspArgs.V,tourInitMethod,rng);
    Tour a2 = generateInitialSolution(tspArgs.V,tourInitMethod,rng);
    Tour a3 = generateInitialSolution(tspArgs.V,tourInitMethod,rng);
    mt19937 rng1(rngSEED);
    Tour b0 = generateInitialSolution(tspArgs.V,tourInitMethod,rng1);
    Tour b1 = generateInitialSolution(tspArgs.V,tourInitMethod,rng1);
    Tour b2 = generateInitialSolution(tspArgs.V,tourInitMethod,rng1);
    Tour b3 = generateInitialSolution(tspArgs.V,tourInitMethod,rng1);

    a0.setCost(tspArgs.V);
    a1.setCost(tspArgs.V);
    a2.setCost(tspArgs.V);
    a3.setCost(tspArgs.V);
    b0.setCost(tspArgs.V);
    b1.setCost(tspArgs.V);
    b2.setCost(tspArgs.V);
    b3.setCost(tspArgs.V);

    cout << " a0 : " << a0.getCost() << " b0 : " << b0.getCost() << endl;
    cout << " a1 : " << a1.getCost() << " b1 : " << b1.getCost() << endl;
    cout << " a2 : " << a2.getCost() << " b2 : " << b2.getCost() << endl;
    cout << " a3 : " << a3.getCost() << " b3 : " << b3.getCost() << endl;

/*
    //========== F2OPTB score test ===========
    double F2OPTB_spentSec = 0;
    time_t F2OPTB_start = clock();
    double F2OPTB_bestScore = DBL_MAX;
    string LOCAL_METHOD = "F2OPT";
    while(F2OPTB_spentSec < secLmt){
      Tour initTour = generateInitialSolution(tspArgs);
      Tour optTour = searchLocalOpt(tspArgs.V, LOCAL_METHOD,initTour);
      optTour.setCost(tspArgs.V);
    
      if(F2OPTB_bestScore > optTour.getCost()){
        F2OPTB_bestScore = optTour.getCost();
      }

      F2OPTB_spentSec = (double)(clock() - F2OPTB_start) / CLOCKS_PER_SEC;
    }
    cout << "F2OPTB : best -> " << F2OPTB_bestScore << endl;
*/

/*
  //============ Hyper Parameter searching =========
  cout << "Start time : ";
  printTime();

  string fileName = "hyperParameterSearchRstRD100.csv";
  ofstream rst_file(fileName);

  // write the fist line
  rst_file << "Best Score" << ", "<<"SEED"<<", " << "HMIN" << ", " << "HMAX" << ", " << "T" << ", " << "TMAX" << ", " << "MMAX" << ", " << "alpha" << ", " << "beta" << ", " << "gamma" << ", " << "LAMBDA" << endl;

  vector<int> hmin_vec = {3,3,3,3,3,4,4,4,4,5,5,5,6,6,7};
  vector<int> hmax_vec = {3,4,5,6,7,4,5,6,7,5,6,7,6,7,7};
  vector<int> K_vec = {91,363,1013,2345,4795,273,923,2255,4705,651,1983,4433,1333,3783,2451};

  double secLmt = 60.0; // time limit of each experiment
  int experi_num = 600; // totally run for experi_num minutes

  Arguments* tspArgs;
  LinearFittedQIteration* LinQ;

  vector<string> argSTR = { TSPLIB_instances[1], tourInitMethod, learnTermiCondi, thetaInitMethod};

  for(int experi = 0;experi <experi_num; experi++){
    cout << "experiment : " << experi << " start"<< endl;
    int hminmax_key = genrand_int31() % 15;
    HMIN = hmin_vec.at(hminmax_key);
    HMAX = hmax_vec.at(hminmax_key);
    int K = K_vec.at(hminmax_key);

    T = (int)(K * (1 + genrand_real3()*2));
    TMAX = (int)(T * (1 + genrand_real3()*2)) + 30;
    MMAX = (int)(TMAX * (1 + genrand_real3()*2)) + 30;

    alpha = 1 + genrand_real3()*3;
    beta = genrand_real3();

    gamma = genrand_real3();
    LAMBDA = genrand_int31()%20 + 2;

    SEED ++;

    vector<int> argINT = {T, TMAX, MMAX, LAMBDA,HMIN,HMAX, SEED,EPILMT};
    vector<double> argREA = {alpha, beta, gamma, thetaInitPara, greedyEps, secLmt};

    tspArgs = new Arguments(argSTR,argINT,argREA);
    LinQ = new LinearFittedQIteration(*tspArgs);
    LinQ->learn(*tspArgs);

    rst_file << LinQ->bestTour.getCost() << ", "<< SEED << ", " <<HMIN << ", " << HMAX << ", " << T << ", " << TMAX << ", " << MMAX << ", " << alpha << ", " << beta << ", " << gamma << ", " << LAMBDA << endl;

    delete tspArgs;
    delete LinQ;
  }

  rst_file.close();

  cout << "End time : ";
  printTime();
*/

  return 0;
}