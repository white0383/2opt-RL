#include <iostream>
#include <string>
#include <vector>
#include <map>

// includes
#include "./model/Arguments.h"
//#include "./RLmodel/LinearFittedQIteration.h"
#include "./helper/FileIOHelper.h"

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
"a280", "eil51", "ida8197", "lin318", "pcb3038", "pr144", "rd100", "ulysses16",
"ali535", "burma14", "dbj2924", "eil76", "gr137", "kroA100", "linhp318", "pcb442", "pr152", "rd400", "ts225", "ulysses22",
"att48", "ch130", "dea2382", "fdp3256", "kroA150", "lsm2854", "pia3056", "pr226", "rl11849", "tsp225", "usa13509",
"att532", "ch150", "dga9698", "fjr3672", "gr202", "kroA200", "lsn3119", "pka379", "pr2392", "rl1304", "vm1084",
"d1291", "dhb3386", "fjs3649", "kroB100", "lta3140", "pla33810", "pr264", "rl1323", "u1060", "vm1748",
"d15112", "dka1376", "fl1400", "gr229", "kroB150", "ltb3729", "pla7397", "pr299", "rl1889", "u1432", "xmc10150",
"bch2762", "d1655", "dkc3938", "fl1577", "kroB200", "nrw1379", "pla85900", "pr439", "rl5915", "u159", "xqe3891",
"beg3293", "d18512", "dke3097", "fl3795", "gr431", "kroC100", "p654", "pma343", "pr76", "rl5934", "u1817", "xqf131",
"berlin52", "d198", "dkf3954", "fl417", "kroD100", "pr1002", "rat195", "u2152", "xqg237",
"bier127", "d2103", "dlb3694", "fnl4461", "kroE100", "pbd984", "pr107", "rat575", "u2319", "xql662",
"d493", "dsj1000", "gr96", "lap7454", "pbn423", "pr124", "rat783", "u574", "xsc6880",
"brd14051", "d657", "eil101", "gil262","lin105", "pcb1173", "pr136", "rat99", "st70", "u724", "xua3937"
};

int main(){
  /************************************/
  /**** Set your test config here. ****/
  /************************************/
  //**** Methods
  vector<string> TSPLIB_instances = {"eil51", "berlin52", "st70"};
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
  unsigned int SEED =  12080300;
  unsigned int EPILMT = 50;
  double secLmt = 180.0;

  /* use below while implementation */
  //"ArgumentsDebug" "a280"
  vector<string> argSTR = {"ArgumentsDebug", tourInitMethod, learnTermiCondi, thetaInitMethod};
  vector<unsigned int> argINT = {T, TMAX, MMAX, LAMBDA,HMIN,HMAX, SEED,EPILMT};
  vector<double> argREA = {alpha, beta, gamma, thetaInitPara, greedyEps, secLmt};

  Arguments tspArgs = Arguments(argSTR, argINT, argREA);
  //LinearFittedQIteration LinQ = LinearFittedQIteration(tspArgs);
  //LinQ.learn(tspArgs);
  /*********************************/

  /* use below in real experiments */
  // Nothing yet

  return 0;
}