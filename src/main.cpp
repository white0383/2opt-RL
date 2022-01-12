#include <iostream>
#include <string>
#include <vector>
#include <map>

// includes MUST
#include "./model/Arguments.h"
#include "./RLmodel/LinearFittedQIteration.h"
#include "./solver/local_search/TestLocalOpt.h"

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
  vector<string> TSPLIB_KRO100s = {"kroA100", "kroB100", "kroC100", "kroD100", "kroE100"};
  vector<string> VLSI_instances = {"xqf131", "xqg237", "pma343","xql662"};
  string tourInitMethod = "RT"; // RT FI
  string learnTermiCondi = "SEC"; // EPI SEC
  string thetaInitMethod = "ONE"; // UNI GAU ONE

  //**** Learning Hyperparameters
  int T = 100; // the number of samples for each learning
  int TMAX = 300; // the number of steps per theta update
  int MMAX = 1000; // maximum size of MDP queue
  int LAMBDA = 13; // parameter in calculating rho
  int HMIN = 3; // minimum axis divisions in partition
  int HMAX = 6; // maximum axis divisions in partition
  double alpha = 4.0; // in reward of MDP
  double beta = 0.5; // in reward of MDP
  double gamma = 0.35; // discount factor
  double thetaInitPara = -1.0; 
  double greedyEps = 0.1; // probability of random action

  //**** Other Hyperparameters
  int SEED = 1070466; // Jan 7th 01:23
  int EPILMT = 50;
  double secLmt = 60.0;

  /*********************************/
  /***** Experiment code below *****/
  /*********************************/
    //vector<string> argSTR = {TSPLIB_KRO100s[0], tourInitMethod, learnTermiCondi, thetaInitMethod};
    //vector<int> argINT = {T, TMAX, MMAX, LAMBDA,HMIN,HMAX, SEED,EPILMT};
    //vector<double> argREA = {alpha, beta, gamma, thetaInitPara, greedyEps, secLmt};
    //Arguments tspArgs = Arguments(argSTR,argINT,argREA);
    //LinearFittedQIteration LinQ = LinearFittedQIteration(tspArgs);
    //LinQ.learn(tspArgs);

/* argmax to argmin */
/* just times -1 to Q function !!*/
/* big TSP*/
    alpha = 100000.0;
    beta = 0.5;
    gamma = 0.35;
    LAMBDA = 13;

    vector<int> SEEDs = {1245}; // more to ten ?
    vector<string> TSPs = {"kroA100"}; // {"kroA100", "kroB100", "kroC100", "kroD100", "kroE100"};
    vector<int> Ts = {3200};
    vector<int> HMINs = {4};
    HMIN = HMINs.front();
    HMAX = 6;
    secLmt = 60.0; // one min
    learnTermiCondi = "EPI";
    EPILMT = 20;
    thetaInitPara = 1.0; // make first theta best improvement
    greedyEps = 0.1;
    string localMethod = "F2OPTM";
    
    //====== output file data
    string rstFileDir  = "../result/rawdata/";
    string rstFileHead = "superBigAlphaRst";
    string rstFileTail = ".csv";

    string logFileDir  = "../result/log/";
    string logFileHead = "superBigAlphaLog";
    string logFileTail = ".csv";
    //=======================

    Arguments* tspArgs;
    LinearFittedQIteration* LinQ;
    LocalSearchTester* LStester;

    vector<string> argSTR = { TSPLIB_instances[0], tourInitMethod, learnTermiCondi, thetaInitMethod};
    vector<int> argINT = {T, TMAX, MMAX, LAMBDA,HMIN,HMAX, SEED,EPILMT};
    vector<double> argREA = {alpha, beta, gamma, thetaInitPara, greedyEps, secLmt};

    for(string TSP : TSPs){
      argREA[5] = secLmt;
      argSTR[0] = TSP;
      string rstFileName = rstFileDir + rstFileHead + "_" +TSP + rstFileTail;
      ofstream rst_file(rstFileName);
      rst_file << "HMIN , T, SEED, bestScore, tourCount, ite, epi, learn, action, reward, mdp, update, reBu, state, bestInfo, feaVec, theta, dataSec, CPLEX, TimeLine \n";

      for(int seed : SEEDs){
        argINT[6] = seed;
        tspArgs = new Arguments(argSTR, argINT, argREA);
        for(int t : Ts){
          tspArgs->T = t; // 1 : 2 : 6 rule
          tspArgs->TMAX = t * 2;
          tspArgs->MMAX = t * 6;

          for(int hmin : HMINs){
            string logFileName = logFileDir + logFileHead + "_" + TSP + "_" + to_string(seed) + "_" + to_string(t) + "_" + to_string(hmin) + rstFileTail;
            ofstream log_file(logFileName);
            //======== RL experiment part ===========
            tspArgs->HMIN = hmin;
            tspArgs->resetRNG();
            cout << TSP << " " << seed << " " << t << " " << hmin << endl;

            LinQ = new LinearFittedQIteration(*tspArgs);
            LinQ->learn(*tspArgs);
            // print LinQ info
            rst_file << tspArgs->HMIN << " ,";
            rst_file << tspArgs->T << " ,";
            rst_file << tspArgs->SEED << " ,";
            rst_file << LinQ->bestTour.getCost() << " ,";
            rst_file << LinQ->initTourCount << " ,";
            rst_file << LinQ->time << " ,";
            rst_file << LinQ->epi << " ,";
            for(time_t time : LinQ->timeVec){
              rst_file << (double)time/CLOCKS_PER_SEC << " ,";
            }
            for(pair<int,double> p : LinQ->bestScoreVec){
              rst_file << p.first << " ," << p.second << " ,";
            }
            rst_file << "\n";

            // print event log
            log_file << "Event, epi, step, time, tourCount, swapCount, nowScore, bestScore, sec \n"; 
            for(auto log : LinQ->eventVec){
              log.print(log_file);
            }

            //===================================
            log_file.close();
            delete LinQ;
          }
        }

        tspArgs->resetRNG();
        LStester = new LocalSearchTester(*tspArgs,localMethod);
        LStester->setPr(greedyEps);
        LStester->run(*tspArgs);

        // print LStester info
        rst_file << localMethod <<" ," << " ," << seed << " ," << LStester->bestScore << " ," << LStester->initTourCount << "  ";
        for(int i=0;i<15;i++){
          rst_file << " ,";
        } 
        for(pair<int,double> p : LStester->bestScoreVec){
          rst_file << p.first << " ," << p.second << " ,";
        }
        rst_file << "\n";

        // print event log
        string logFileName = logFileDir + logFileHead + "_" + TSP + "_" + to_string(seed) + "_" + localMethod + rstFileTail;
        ofstream log_file(logFileName);
        log_file << "tourCount, initScore, optScore, bestScore, sec\n";
        for(auto log : LStester->eventVec){
          log.print(log_file);
        }

        delete tspArgs;
        delete LStester;
        log_file.close();
      }

      rst_file.close();
    }

  return 0;
}