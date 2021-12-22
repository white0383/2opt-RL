#include <vector>
#include <string>
#include <iostream>
#include <algorithm> // std::find std::min
#include <cmath> // floor, log
#include "./Arguments.h"
#include "../model/Graph.h"
#include "../helper/FileIOHelper.h"
#include "../helper/mt19937ar.h"

using namespace std;

#define STRINUM 6
#define INTENUM 6
#define REALNUM 6

vector<string> INIT_SOLUTION_METHOD_LIST = {
  "RT", //Random Tour
  "NN", //Nearest Neighbor method
  "FI", //Farthest Insertion method
  //----- not yet implemented (2021 11 13)---------
  "NA", //Nearest Addition method (prim method like)
  "NI", //Nearest Insertion method
  "CI", //Cheapest Insertion method
  "RI", //Random Insertion method
  "GA", //Greedy Algorithm (kuskal method like)
  "CW", //Clarke-Wright method (saving method)
  "CF", //ChristoFides algorithm
  "CH"  //Convex Hull insertion
};

vector<string> LOCAL_SEARCH_METHOD_LIST {
  "2OPT", //2-opt -> search neighborhood in sequence
  "F2OPT", //fast 2-opt -> improved 2-opt suggested by haraguchi sensei
  "SF2OPT", //scaled fast 2-opt -> fast 2-opt using scaled distance
  //----- not yet implemented (2021 11 13)---------
  "3OPT", //3-opt
  "LK" //Lin–Kernighan method
};

vector<string> TERMINATE_METHOS_LIST {
  "SEC", //computation second
  "EPI"  //maximum episode
};

vector<string> ACTION_GREEDY_METHOD_LIST {
  "FULL", // ActionHelper::genFullGreedy
  "PART", // ActionHelper::genPartGreedy
  "SAMP"  // ActionHelper::genSampleGreedy
};

vector<string> WEIGHT_INIT_METHOD_LIST {
  "UNIFM", // Uniform distribution  U(-para/2,para/2)
  "GAUSS"  // Gaussian distribution N(0,para) = N(mean, stddev)
};

bool verifyInputVectorLength(vector<string>& stringArgs, vector<unsigned int>& integerArgs, vector<double>& realArgs){
  return (stringArgs.size() == STRINUM) && (integerArgs.size() == INTENUM) && (realArgs.size() == REALNUM);
}

//return true if method is in methodList
bool verifyMethodInMethodList(string& method, vector<string>& methodList){
  return find(methodList.begin(),methodList.end(),method) != methodList.end();
}

//return true if num is in [0,1]
bool verifyNumInZeroOne(double num){
  return (num >= 0) && (num <= 1);
}

bool verifyInputData(vector<string>& stringArgs, vector<unsigned int>& integerArgs, vector<double>& realArgs){
  try{
    //Check input vectors' length
    if(verifyInputVectorLength(stringArgs, integerArgs, realArgs) == false) {
      string expName = "ERROR : improper input vectors' length";
      string expDesc = "please check your input parameter has";
      string expDesc2 = "required : 6 strings, 6 unsigned int, 5 double";
      string expDesc3 = "yout input has " + to_string(stringArgs.size()) + " strings, " + to_string(integerArgs.size()) + " unsigned int, " + to_string(realArgs.size()) + " double";
      vector<string> expVec = {expName,expDesc,expDesc2,expDesc3};
      throw expVec;
    };

    //Check name of init-solution generation method
    string INIT_SOLUTION_METHOD = stringArgs[1];
    if(verifyMethodInMethodList(INIT_SOLUTION_METHOD,INIT_SOLUTION_METHOD_LIST) == false){
      string expName = "ERROR : improper init solution method name";
      string expDesc = "please check 2th string argument";
      string expDesc2 = "your input is " + INIT_SOLUTION_METHOD;
      string expDesc3 = "allowed input : RT NN FI";
      vector<string> expVec = {expName,expDesc,expDesc2,expDesc3};
      throw expVec;
    };

    //Check name of local search method
    string LOCAL_SEARCH_METHOD = stringArgs[2];
    if(verifyMethodInMethodList(LOCAL_SEARCH_METHOD,LOCAL_SEARCH_METHOD_LIST) == false){
      string expName = "ERROR : improper local search method name";
      string expDesc = "please check 3th string argument";
      string expDesc2 = "your input is " + LOCAL_SEARCH_METHOD;
      string expDesc3 = "allowed input : 2OPT F2OPT";
      vector<string> expVec = {expName,expDesc,expDesc2,expDesc3};
      throw expVec;
    };

    //Check name of termination method
    string TERMINATE_METHOD = stringArgs[3];
    if(verifyMethodInMethodList(TERMINATE_METHOD,TERMINATE_METHOS_LIST) == false){
      string expName = "ERROR : improper terminate method name";
      string expDesc = "please check 4th string argument";
      string expDesc2 = "your input is " + TERMINATE_METHOD;
      string expDesc3 = "allowed input : EPI SEC";
      vector<string> expVec = {expName,expDesc,expDesc2,expDesc3};
      throw expVec;
    };

    //Check name of greedy method in action constructor
    string ACTION_GREEDY_METHOD = stringArgs[4];
    if(verifyMethodInMethodList(ACTION_GREEDY_METHOD,ACTION_GREEDY_METHOD_LIST) == false){
      string expName = "ERROR : improper action greedy method name";
      string expDesc = "please check 5th string argument";
      string expDesc2 = "your input is " + ACTION_GREEDY_METHOD;
      string expDesc3 = "allowed input : FULL PART SAMP";
      vector<string> expVec = {expName,expDesc,expDesc2,expDesc3};
      throw expVec;
    }

    //Check name of weight vector generation method
    string WEIGHT_INIT_METHOD = stringArgs[5];
    if(verifyMethodInMethodList(WEIGHT_INIT_METHOD, WEIGHT_INIT_METHOD_LIST) == false){
      string expName = "ERROR : improper weight initialization method name";
      string expDesc = "please check 6th string argument";
      string expDesc2 = "your input is " + WEIGHT_INIT_METHOD;
      string expDesc3 = "allowed input : UNIFM GAUSS";
      vector<string> expVec = {expName,expDesc,expDesc2,expDesc3};
      throw expVec;
    }

    //Check T < TMAX < MMAX
    unsigned int T = integerArgs[1];
    unsigned int TMAX = integerArgs[2];
    unsigned int MMAX = integerArgs[5];
    if((T >= TMAX) || (TMAX >= MMAX)){
      string expName = "ERROR : improper T, TMAX, MMAX value";
      string expDesc = "please check 2,3,6th unsigned int argument";
      string expDesc2 = "your input is T : " + to_string(T) + " TMAX : " + to_string(TMAX) + " MMAX : " + to_string(MMAX);
      string expDesc3 = "it should be T < TMAX < MMAX";
      vector<string> expVec = {expName,expDesc,expDesc2,expDesc3};
      throw expVec;
    }

    //Check GAMMA, ALPHA, EPS in [0,1]
    double GAMMA = realArgs[0];
    double ALPHA = realArgs[1];
    double EPS = realArgs[4];
    if(!(verifyNumInZeroOne(GAMMA) && verifyNumInZeroOne(ALPHA) && verifyNumInZeroOne(EPS))){
      string expName = "ERROR : improper GAMMA, ALPHA, EPS value";
      string expDesc = "please check 1,2,5th double argument";
      string expDesc2 = "your input is GAMMA : " + to_string(GAMMA) + " ALPHA : " + to_string(ALPHA) + " EPS : " + to_string(EPS);
      string expDesc3 = "All of them are in [0,1]";
      vector<string> expVec = {expName,expDesc,expDesc2,expDesc3};
      throw expVec;     
    }

    //Check parametor of weight vector initialization and SF2OPT are non-negative
    double WEIGHTS_INITPARA = realArgs[3];
    double EPS_SF2OPT = realArgs[5];
    if((WEIGHTS_INITPARA < 0 ) || (EPS_SF2OPT < 0)){
      string expName = "ERROR : improper WEIGHTS_INITPARA or EPS_SF2OPT value";
      string expDesc = "please check 4,6th double argument";
      string expDesc2 = "your input is WEIGHTS_INITPARA : " + to_string(WEIGHTS_INITPARA) + " EPS_SF2OPT : " + to_string(EPS_SF2OPT);
      string expDesc3 = "they should be non-negative";
      vector<string> expVec = {expName,expDesc,expDesc2,expDesc3};
      throw expVec;  
    }
  } catch(vector<string> expS) {
    for(string exp : expS) cout << exp << endl;
    exit(1);
  }
  return true;
}

Arguments::Arguments() {
  cout << "ERROR : Empty Arguments constructor is not allowed" << endl;
  exit(1);
}

Arguments::Arguments(vector<string>& stringArgs, vector<unsigned int>& integerArgs, vector<double>& realArgs){
  bool tmpBool = verifyInputData(stringArgs,integerArgs,realArgs);

  if(verifyInputData(stringArgs,integerArgs,realArgs)){
    this->TSP_DATA_FILE = stringArgs[0];
    this->INIT_SOLUTION_METHOD = stringArgs[1];
    this->LOCAL_SEARCH_METHOD = stringArgs[2];
    this->TERMINATE_METHOD = stringArgs[3];
    this->ACTION_GREEDY_METHOD = stringArgs[4];
    this->WEIGHT_INIT_METHOD = stringArgs[5];

    this->SEED = integerArgs[0];
    this->T = integerArgs[1];
    this->TMAX = integerArgs[2];
    this->THETA = integerArgs[3];
    this->EPI_LIMIT = integerArgs[4];
    this->MMAX = integerArgs[5];

    this->GAMMA = realArgs[0];
    this->ALPHA = realArgs[1];
    this->SEC_LIMIT = realArgs[2];
    this->WEIGHTS_INITPARA = realArgs[3];
    this->EPS = realArgs[4];
    this->EPS_SF2OPT = realArgs[5];

    init_genrand(SEED);

    vector< vector <int> > nodes = readTSPFile(this->TSP_DATA_FILE);
    this->V = Graph(nodes);
    this->KSMP = min(this->V.getN(),100);
    this->OMEGA = min(this->V.getN(),100);
    this->K = 3+ this->KSMP + this->OMEGA + 6;
    this->ASMP = min(this->V.getN(),100);
    this->SIGMAMAX = 15;//floor(log(this->V.getN()) / log(2));//min(this->V.getN()/10, 100);
    this->F3IMAX = floor(log(this->V.getN()) / log(2));

  } else {
    cout << "Unexpected exception Occured in Arguments constructor" << endl;
    cout << "Terminate TSP-RL" << endl;
    exit(1);
  }
}