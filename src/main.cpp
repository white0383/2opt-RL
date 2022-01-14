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

vector<string> EXPERI_MINI_FILES = {
"burma14", "att48", "eil51", "berlin52", "st70",
"rat99", "rd100", "kroA100", "pr152", "u159", 
"kroA200", "gr229", "gil262", "pr264", "a280", 
"pr299", "rd400", "gr431", "pr439","rat575", 
"vm1084", "d1655"
};



vector<string> EXPERI_FULL_FILES = {
"burma14", "ulysses16", "ulysses22", "att48", "eil51", "berlin52", "st70", "eil76", "pr76", "gr96", 
"rat99", "rd100", "kroA100", "kroB100", "kroC100", "kroD100", "kroE100", "eil101", "lin105", "pr107", 
"pr124", "bier127", "ch130", "xqf131", "pr136", "gr137", "pr144", "kroA150", "ch150", "kroB150", 
"pr152", "u159", "rat195", "d198", "kroA200", "kroB200", "gr202", "ts225", "tsp225", "pr226", 
"gr229", "xqg237", "gil262", "pr264", "a280", "pr299", "lin318", "linhp318", "pma343", "pka379", 
"rd400", "fl417", "pbn423", "gr431", "pr439", "pcb442", "d493", "att532", "ali535", "u574", 
"rat575", "p654", "d657", "xql662", "u724", "rat783", "pbd984", "dsj1000", "pr1002", "u1060", 
"vm1084", "pcb1173", "d1291", "rl1304", "rl1323", "dka1376", "nrw1379", "fl1400", "u1432", "fl1577", 
"d1655", "vm1748", "u1817", "rl1889", "d2103", "u2152", "u2319", "dea2382", "pr2392", "bch2762"
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
  string learnTermiCondi = "SEC"; // EPI SEC TOUR
  string thetaInitMethod = "ONE"; // UNI GAU ONE

  //** Default setting
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
  int TOURLMT = 200;

  /*********************************/
  /***** Experiment Code begin *****/
  /*********************************/
  /* thesis Experiment mini */
    alpha = 100000.0;
    beta = 0.5;
    gamma = 0.35;
    LAMBDA = 13;
    HMIN = 4;
    HMAX = 6;
    T = 3200;
    TMAX = T * 2;
    MMAX = T * 6;
    learnTermiCondi = "TOUR";
    TOURLMT = 200;
    thetaInitPara = 1.0; // make first theta best improvement
    greedyEps = 0.1;

    vector<string> localMethods = {"F2OPT", "F2OPTB", "F2OPTM"};
    vector<int> SEEDs = {2844, 5765, 8471, 9677, 6414};
    vector<string> TSPs = {"pr299", "rd400", "gr431", "pr439","rat575", 
"vm1084", "d1655"};//EXPERI_MINI_FILES;
    
    //====== output file data
    string experiName = "thesisExpMini";
    string rstFileDir  = "../result/rawdata/" + experiName + "/";
    string rstFileHead = "RST";
    string rstFileTail = ".csv";

    string logFileDir  = "../result/log/" + experiName + "/";
    string logFileHead = "LOG";
    string logFileTail = ".csv";

    string thetaFileDir  = "../result/theta/" + experiName + "/";
    string thetaFileHead = "THETA";
    string thetaFileTail = ".csv";
    //=======================

    Arguments* tspArgs;
    LinearFittedQIteration* LinQ;
    LocalSearchTester* LStester;

    vector<string> argSTR = { TSPLIB_instances[0], tourInitMethod, learnTermiCondi, thetaInitMethod};
    vector<int> argINT = {T, TMAX, MMAX, LAMBDA,HMIN,HMAX, SEED,TOURLMT};
    vector<double> argREA = {alpha, beta, gamma, thetaInitPara, greedyEps, secLmt};

    for(string TSP : TSPs){
      argSTR[0] = TSP;
      string rstFileName = rstFileDir + rstFileHead + "_" +TSP + rstFileTail;
      ofstream rst_file(rstFileName);
      rst_file << "method, SEED, best, swaps, epi, action, dataSet, CPLEX, etc, spendSec, TimeLine \n";

      for(int seed : SEEDs){
        argINT[6] = seed;
        tspArgs = new Arguments(argSTR, argINT, argREA);

        //======== RL experiment part begin ===========
        tspArgs->resetRNG();
        cout << TSP << " " << seed << "RL begin" <<endl;
        printTime();

        LinQ = new LinearFittedQIteration(*tspArgs);
        LinQ->learn(*tspArgs);
        // print LinQ info
        rst_file << "RL ,";                                             // method
        rst_file << tspArgs->SEED << " ,";                              // SEED
        rst_file << LinQ->bestTour.getCost() << " ,";                   // best
        rst_file << LinQ->time << " ,";                                 // swaps
        rst_file << LinQ->epi << " ,";                                  // epi
        rst_file << (double)LinQ->timeVec.at(1)/CLOCKS_PER_SEC << " ,"; // action
        rst_file << (double)LinQ->timeVec.at(10)/CLOCKS_PER_SEC << " ,";// dataset
        rst_file << (double)LinQ->timeVec.at(11)/CLOCKS_PER_SEC << " ,";// CPLEX
        time_t tmpSumEtc = LinQ->timeVec.at(0) - LinQ->timeVec.at(1) - LinQ->timeVec.at(10) - LinQ->timeVec.at(11);
        rst_file << (double)tmpSumEtc / CLOCKS_PER_SEC << " ,";         // etc
        rst_file << (double)LinQ->timeVec.at(0)/CLOCKS_PER_SEC << " ,"; // spendSec
        for(pair<int,double> p : LinQ->bestScoreVec){                   // TimeLine
          rst_file << p.first << " ," << p.second << " ,";
        }
        rst_file << "\n";

        // print event log
        string logFileName = logFileDir + logFileHead + "_" + TSP + "_" + to_string(seed) + logFileTail;
        ofstream log_file(logFileName);
        log_file << "Event, epi, step, time, tourCount, swapCount, nowScore, bestScore, sec \n"; 
        for(auto log : LinQ->eventVec){
          log.print(log_file);
        }
        log_file.close();

        // print theta
        string thetaFileName = thetaFileDir + thetaFileHead + "_" + TSP + "_" + to_string(seed) + thetaFileTail;
        ofstream theta_file(thetaFileName);
        theta_file << "index , value \n";
        int i_theta = 0;
        for(auto val : LinQ->theta){
          theta_file << i_theta << " ," << val << "\n";
          i_theta++;
        }

        delete LinQ;
        //======== RL experiment part end ===========

        for(string localMethod : localMethods){
          cout << TSP << " " << seed << " "<< localMethod<<" begin" <<endl;
          printTime();
          tspArgs->resetRNG();
          LStester = new LocalSearchTester(*tspArgs, localMethod);
          if(localMethod == "F2OPTM") LStester->setPr(greedyEps);
          LStester->run(*tspArgs);

          // print rst file
//"method, SEED, best, swaps, epi, action, dataSet, CPLEX, etc, spendSec, TimeLine \n";
          rst_file << localMethod << " ,";          // method
          rst_file << seed << " ,";                 // seed
          rst_file << LStester->bestScore << " ,";  // best
          rst_file << LStester->totalSwapNum<<" ,"; // swaps
          rst_file << " ,";                         // epi
          rst_file << " ,";                         // action
          rst_file << " ,";                         // dataSet
          rst_file << " ,";                         // CPLEX
          rst_file << " ,";                         // etc
          rst_file << LStester->spendSec << " ,";   // spendSec
          for(pair<int,double> p : LStester->bestScoreVec){
            rst_file << p.first << " ," << p.second << " ,";
          }
          rst_file << "\n";

          //print log file
          string logFileName = logFileDir + logFileHead + "_" + TSP + "_" + to_string(seed) + "_" + localMethod + logFileTail;
          ofstream log_file(logFileName);
          log_file << "tourCount, swapNum,initScore, optScore, bestScore, sec\n";
          for(auto log : LStester->eventVec){
            log.print(log_file);
          }
          log_file.close();

          delete LStester;
        }
        delete tspArgs;
      } // end for seed
      rst_file.close();
    } // end for TSP

  // ======== Experiment Code end =======
  return 0;
}