#include <iostream>
#include <string>
#include <vector>
#include <map>

// includes
#include "./model/Arguments.h"
//#include "./RLmodel/LinearFittedQIteration.h"
#include "./helper/FileIOHelper.h"

// includes for debugging
#include <ctime>
#include "./model/Tour.h"
#include "./solver/initial_solution/GenerateInitialSolution.h"
#include "./solver/local_search/SearchLocalOpt.h"
#include "./solver/local_search/method/TwoOpt.h"
#include "./solver/local_search/method/FastTwoOpt.h"
#include "./solver/local_search/method/tmp.h"

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
  string tourInitMethod = "RT"; // RT FI
  string learnTermiCondi = "EPI"; // EPI SEC
  string thetaInitMethod = "UNI"; // UNI GAU ONE

  //**** Learning Hyperparameters
  unsigned int T = 100; // the number of samples for each learning
  unsigned int TMAX = 200; // the number of steps per theta update
  unsigned int MMAX = 600; // maximum size of MDP queue
  unsigned int LAMBDA = 5; // parameter in calculating rho
  unsigned int HMIN = 3; // minimum axis divisions in partition
  unsigned int HMAX = 4; // maximum axis divisions in partition
  double alpha = 1.1; // in reward of MDP
  double beta = 0.95; // in reward of MDP
  double gamma = 0.95; // discount factor
  double thetaInitPara = 2.0; 
  double greedyEps = 0.1; // probability of random action

  //**** Other Hyperparameters
  unsigned int SEED =  1208030;
  unsigned int EPILMT = 50;
  double secLmt = 180.0;

  /* use below while implementation */
  /*********************************/
  //"ArgumentsDebug" TSPLIB_instances[0]
  vector<string> argSTR = { TSPLIB_instances[4], tourInitMethod, learnTermiCondi, thetaInitMethod};
  vector<unsigned int> argINT = {T, TMAX, MMAX, LAMBDA,HMIN,HMAX, SEED,EPILMT};
  vector<double> argREA = {alpha, beta, gamma, thetaInitPara, greedyEps, secLmt};

  //Arguments tspArgs = Arguments(argSTR, argINT, argREA);

    //test normal 2opt
    string LOCAL_METHOD = "2OPT";
    //Tour initTour = generateInitialSolution(tspArgs);
    //Tour optTour = searchLocalOpt(tspArgs.V,LOCAL_METHOD,initTour);


    //test fast 2opt
    //LOCAL_METHOD = "F2OPT";
    //Tour initTour = generateInitialSolution(tspArgs);
    //Tour optTour = fastTwoOpt(tspArgs.V,initTour);
    //Tour optTourTmp = fastTwoOptTmp(tspArgs.V,initTour);
    //initTour.setCost(tspArgs.V);
    //optTour.setCost(tspArgs.V);
    //optTourTmp.setCost(tspArgs.V);
    //initTour.printTour();
    //optTour.printTour();
    //optTourTmp.printTour();
    
    //compare fast-2opt and normal-2opt
    Arguments* tspArgs;
    int expNum = 1000;
    map<string, double> timeMapFast;
    map<string, double> timeMapNorm;
    map<string, int> winFastScoreMap; // how many times fastTwoOpt won at score
    map<string, int> winFastTimeMap; // how many times fastTwoOpt won at computation time

    for(auto TSPLIB:TSPLIB_instances){
      argSTR[0] = TSPLIB;
      argINT[6] += 1; // SEED += 1

      tspArgs = new Arguments(argSTR,argINT,argREA);

      time_t startT = clock();
      double rst_time_norm = 0;
      double rst_time_fast = 0;
      int fast_win_score = 0;
      int fast_win_time = 0; 
      double spent_time_norm = 0;
      double spent_time_fast = 0;
      for(int i=0;i<expNum;i++){
        argINT[6] += 1; // SEED += 1
        Tour initTour = generateInitialSolution(*tspArgs);

        startT = clock();
        Tour optTourFast = fastTwoOpt(tspArgs->V,initTour);
        spent_time_fast = (double)(clock() - startT) / CLOCKS_PER_SEC;

        startT = clock();
        Tour optTourNorm = twoOpt(tspArgs->V,initTour);
        spent_time_norm = (double)(clock() - startT) / CLOCKS_PER_SEC;

        optTourFast.setCost(tspArgs->V);
        optTourNorm.setCost(tspArgs->V);

        rst_time_fast += spent_time_fast;
        rst_time_norm += spent_time_norm;

        if(spent_time_fast < spent_time_norm) fast_win_time++;
        if(optTourFast.getCost() < optTourNorm.getCost()) fast_win_score++;
      }

      timeMapFast.insert({TSPLIB,rst_time_fast});
      timeMapNorm.insert({TSPLIB,rst_time_norm});
      winFastScoreMap.insert({TSPLIB,fast_win_score});
      winFastTimeMap.insert({TSPLIB,fast_win_time});
    }

    for(auto TSPLIB : TSPLIB_instances){
      cout << TSPLIB << "\t: ";
      cout << timeMapFast.find(TSPLIB)->second << "\t ";
      cout << timeMapNorm.find(TSPLIB)->second << "\t ";
      cout << winFastScoreMap.find(TSPLIB)->second << "\t ";
      cout << winFastTimeMap.find(TSPLIB)->second << "\t ";
      cout << endl;
    }

  //LinearFittedQIteration LinQ = LinearFittedQIteration(tspArgs);
  //LinQ.learn(tspArgs);
  /*********************************/

  /* use below in real experiments */
  // Nothing yet

  return 0;
}