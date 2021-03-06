#include <vector>
#include <string>
#include <iostream>
#include <fstream> // std::ifstream
#include <algorithm> // std::find std::min
#include <utility> // std::pair
#include <cmath> // std::ceil
#include "./Arguments.h"
#include "./Graph.h"
#include "../helper/FileIOHelper.h"
#include "../helper/mt19937ar.h"

using namespace std;

#define STRINUM 4
#define INTENUM 8
#define REALNUM 6

vector<string> INIT_SOLUTION_METHOD_LIST = {
  "RT", //Random Tour
  "FI", //Farthest Insertion method
  //----- not yet implemented (2021 12 25)---------
  "NN", //Nearest Neighbor method
  "NA", //Nearest Addition method (prim method like)
  "NI", //Nearest Insertion method
  "CI", //Cheapest Insertion method
  "RI", //Random Insertion method
  "GA", //Greedy Algorithm (kuskal method like)
  "CW", //Clarke-Wright method (saving method)
  "CF", //ChristoFides algorithm
  "CH"  //Convex Hull insertion
};

vector<string> TERMINATE_METHOS_LIST {
  "SEC", //computation second
  "EPI", //maximum episode
  "TOUR" //meximum init tours
};

vector<string> THETA_INIT_METHOD_LIST {
  "UNI", // Uniform distribution  U(-para/2,para/2)
  "GAU",  // Gaussian distribution N(0,para) = N(mean, stddev)
  "ONE"
};

namespace ArgumentsHelper{
  //return true if num is in [0,1]
  bool verifyNumInZeroOne(double num){
    return (num >= 0) && (num <= 1);
  }

  bool verifyInputVectorLength(vector<string>& stringArgs, vector<int>& integerArgs, vector<double>& realArgs){
    return (stringArgs.size() == STRINUM) && (integerArgs.size() == INTENUM) && (realArgs.size() == REALNUM);
  }

  //return true if method is in methodList
  bool verifyMethodInMethodList(string& method, vector<string>& methodList){
    return find(methodList.begin(),methodList.end(),method) != methodList.end();
  }

  bool verifyInputData(vector<string>& stringArgs, vector<int>& integerArgs, vector<double>& realArgs){
    try{
      //Check input vectors' length
      if(verifyInputVectorLength(stringArgs, integerArgs, realArgs) == false) {
        string expName = "ERROR : improper input vectors' length";
        string expDesc = "please check your input parameter has";
        string expDesc2 = "required : 4 strings, 8 int, 6 double";
        string expDesc3 = "yout input has " + to_string(stringArgs.size()) + " strings, " + to_string(integerArgs.size()) + " int, " + to_string(realArgs.size()) + " double";
        vector<string> expVec = {expName,expDesc,expDesc2,expDesc3};
        throw expVec;
      };

      //Check input tsp instance name if in valid
      string TSP_INSTANCE_NAME = stringArgs[0];
      string TSP_PATH("../data/TSPLIB/");
      TSP_PATH.append(TSP_INSTANCE_NAME);
      TSP_PATH.append(".tsp");
      ifstream in(TSP_PATH);
      if(in.fail()){
        string expName = "ERROR : inproper TSP instance name";
        string expDesc = "please check 1st string argument";
        string expDesc2 = "your input :" + TSP_INSTANCE_NAME;
        vector<string> expVec = {expName,expDesc,expDesc2};
        throw expVec;
      };

      //Check name of init-solution generation method
      string INIT_SOLUTION_METHOD = stringArgs[1];
      if(verifyMethodInMethodList(INIT_SOLUTION_METHOD,INIT_SOLUTION_METHOD_LIST) == false){
        string expName = "ERROR : improper init solution method name";
        string expDesc = "please check 2nd string argument";
        string expDesc2 = "your input is " + INIT_SOLUTION_METHOD;
        string expDesc3 = "allowed input : RT NN FI";
        vector<string> expVec = {expName,expDesc,expDesc2,expDesc3};
        throw expVec;
      };

      //Check name of termination method
      string TERMINATE_METHOD = stringArgs[2];
      if(verifyMethodInMethodList(TERMINATE_METHOD,TERMINATE_METHOS_LIST) == false){
        string expName = "ERROR : improper terminate method name";
        string expDesc = "please check 3rd string argument";
        string expDesc2 = "your input is " + TERMINATE_METHOD;
        string expDesc3 = "allowed input : EPI SEC";
        vector<string> expVec = {expName,expDesc,expDesc2,expDesc3};
        throw expVec;
      };

      //Check name of theta vector generation method
      string THETA_INIT_METHOD = stringArgs[3];
      if(verifyMethodInMethodList(THETA_INIT_METHOD, THETA_INIT_METHOD_LIST) == false){
        string expName = "ERROR : improper theta initialization method name";
        string expDesc = "please check 4th string argument";
        string expDesc2 = "your input is " + THETA_INIT_METHOD;
        string expDesc3 = "allowed input : UNIFM GAUSS";
        vector<string> expVec = {expName,expDesc,expDesc2,expDesc3};
        throw expVec;
      }

      //Check T < TMAX < MMAX
      int T = integerArgs[0];
      int TMAX = integerArgs[1];
      int MMAX = integerArgs[2];
      if((T >= TMAX) || (TMAX >= MMAX)){
        string expName = "ERROR : improper T, TMAX, MMAX value";
        string expDesc = "please check 1,2,3th int argument";
        string expDesc2 = "your input is T : " + to_string(T) + " TMAX : " + to_string(TMAX) + " MMAX : " + to_string(MMAX);
        string expDesc3 = "they should be T < TMAX < MMAX";
        vector<string> expVec = {expName,expDesc,expDesc2,expDesc3};
        throw expVec;
      }

      //LAMBDA = integerArgs[3] is okay

      //Check HMIN <= HMAX
      int HMIN = integerArgs[4];
      int HMAX = integerArgs[5];
      if(HMIN > HMAX){
        string expName = "ERROR : improper HMIN, HMAX value";
        string expDesc = "please check 5,6th int argument";
        string expDesc2 = "your input is HMIN : " + to_string(HMIN) + " HMAX : " + to_string(HMAX);
        string expDesc3 = "they should be HMIN <= HMAX";
        vector<string> expVec = {expName,expDesc,expDesc2,expDesc3};
        throw expVec;      
      }

      //SEED = integerArgs[6] is okay
      //EPILMT = integerArgs[7] is okay

      //Check alpha >= 1, beta,gamma,greedyEps in [0,1]
      double GAMMA = realArgs[2];
      double ALPHA = realArgs[0];
      double EPS = realArgs[4];

      double alpha = realArgs[0];
      double beta = realArgs[1];
      double gamma = realArgs[2];
      double greedyEps = realArgs[4];
      if(!(verifyNumInZeroOne(beta) && verifyNumInZeroOne(gamma) && verifyNumInZeroOne(greedyEps) && (alpha >= 1))){
        string expName = "ERROR : improper alpha,beta,gamma,greedyEps value";
        string expDesc = "please check 1,2,3,5th double argument";
        string expDesc2 = "your input is alpha : " + to_string(alpha) + " beta : " + to_string(beta) + " gamma : " + to_string(gamma) + " greedyEps : " + to_string(greedyEps);
        string expDesc3 = "All of them(except alpha) are in [0,1]";
        string expDesc4 = "alpha should be greater than 1";
        vector<string> expVec = {expName,expDesc,expDesc2,expDesc3,expDesc4};
        throw expVec;     
      }

      //Check parametor of theta vector initialization and time-limit are non-negative
      double theta_INITPARA = realArgs[3];
      double secLmt = realArgs[5];
      //if(!((theta_INITPARA > 0) && (secLmt > 0) )){
      //  string expName = "ERROR : improper theta_INITPARA or secLmt value";
      //  string expDesc = "please check 4,6th double argument";
      //  string expDesc2 = "your input is theta_INITPARA : " + to_string(theta_INITPARA) + " secLmt : " + to_string(secLmt);
      //  string expDesc3 = "they should be non-negative";
      //  vector<string> expVec = {expName,expDesc,expDesc2,expDesc3};
      //  throw expVec;  
      //}
    } catch(vector<string> expS) {
      for(string exp : expS) cout << exp << endl;
      exit(1);
    }
    return true;
  }

  /**
   * Calculate dimention of feature vector
   * when axis-divide is the only partition method
   * 
   * let x is the number of axis division
   * then, there are x^2 partition in [0,1]^2
   * let y is x^2
   * 
   * in the axis-divide partition method, 
   * the number of discriptors for x is same to
   *    y   ( in case of E_i i = 1~y)
   *  + yC2 ( in case of E_jk j = 1~y-1, k = j+1 ~ y)
   *  ----------
   *  = y + (y^2 - y) / 2 = (y^2 + y) / 2
   * 
   * in this case y is x^2
   * so the number of discriptors for x is
   * (x^4 + x^2)/2
   * 
   * feature vector has state part and action part
   * they has same size of (x^4 + x^2)/2
   * 
   * x is integer in [min_div, max_div]
   * 
   * in conclusion,
   * the dimention of feature vector is 
   * 1 + sum_{x in [min_div, max_div]} (x^4 + x^2)
   * 
   * "1" is expression of constant term in linear-Qfunction 
   */
  int calcK0_axisDivide(int min_div, int max_div){
    int rst_K0 = 0;

    for(int x = min_div ; x <= max_div ; x++){
      rst_K0 += (x*x*x*x + x*x) / 2; // (x^4 + x^2)
    }

    return rst_K0;
  } 

  bool compare(pair<int,double> a, pair<int,double> b){
    if (a.second == b.second) {
      return a.first < b.first;
    } else {
      return a.second < b.second;
    }
  }

  int checkPartitionOnAxis(double x, int h){
    if(x == 0){
      return 1;
    } else {
      return (int)ceil(x * (double)h);
    }
  } 

  int checkPartition(Node& node, int h){
    int x_partitionOnAxis = checkPartitionOnAxis(node.x,h);
    int y_partitionOnAxis = checkPartitionOnAxis(node.y,h);

    return x_partitionOnAxis + h * (y_partitionOnAxis - 1);
  }
} // namespace ArgumentsHelper

Arguments::Arguments() {
  cout << "ERROR : Empty Arguments constructor is not allowed" << endl;
  exit(1);
}

Arguments::Arguments(vector<string>& stringArgs, vector<int>& integerArgs, vector<double>& realArgs):
  rng_tourInit(integerArgs[6])
{
  if(ArgumentsHelper::verifyInputData(stringArgs,integerArgs,realArgs)){
    // copy input datas
    this->TSP_INSTANCE_NAME = stringArgs[0];
    this->TOUR_INIT_METHOD = stringArgs[1];
    this->LEARN_TERMINATE_METHOD = stringArgs[2];
    this->THETA_INIT_METHOD = stringArgs[3];

    this->T = integerArgs[0];
    this->TMAX = integerArgs[1];
    this->MMAX = integerArgs[2];
    this->LAMBDA = integerArgs[3];
    this->HMIN = integerArgs[4];
    this->HMAX = integerArgs[5];
    this->SEED = integerArgs[6];
    this->EPI_LIMIT = integerArgs[7];

    this->ALPHA = realArgs[0];
    this->BETA = realArgs[1];
    this->GAMMA = realArgs[2];
    this->THETA_INITPARA = realArgs[3];
    this->GREEDY_EPS = realArgs[4];
    this->SEC_LIMIT = realArgs[5];

    // operate and calculate
    init_genrand(this->SEED);
    this->V = Graph(this->TSP_INSTANCE_NAME);
    this->K0 = ArgumentsHelper::calcK0_axisDivide(this->HMIN, this->HMAX);
    this->K = 2*this->K0;
    this->StateBegin = 0;
    this->ActionBegin = K0;
    this->setPartitions();
    this->setPartitionBegins();
    this->setU();
  } else {
    cout << "Unexpected exception Occured in Arguments constructor" << endl;
    cout << "Terminate 2opt-RL" << endl;
    exit(1);
  }
}

void Arguments::setPartitions(){
  this->partitions.clear();

  vector<int> partition(this->V.nodes.size());
  for(int h = this->HMIN; h <= this->HMAX ; h++){
    for(int i = 0 ; i < this->V.nodes.size();i++){
      partition[i] = ArgumentsHelper::checkPartition(this->V.nodes[i],h);
    }
    this->partitions[h]=partition;
  }
}

void Arguments::setPartitionBegins(){
  this->partitionBegins.clear();

  int partitionBegin = 0;
  for(int h = this->HMIN; h<= this->HMAX ; h++){
    this->partitionBegins[h] = partitionBegin;

    partitionBegin += (h*h*h*h + h*h) /2;
  }
}

void Arguments::setU(){
  double rst_maxDIJ = -1;
  int farthestNodeFromI = 0;
  double farthestDistFromI = 0;
  for(int i=1;i<=this->V.getN();i++){
    farthestNodeFromI = this->V.distOrder[i][this->V.getN()-1];
    farthestDistFromI = this->V.distMatrix[i][farthestNodeFromI];
    if(rst_maxDIJ < farthestDistFromI){
      rst_maxDIJ = farthestDistFromI;
    }
  }

  double rst_U = rst_maxDIJ * this->V.getN() ;
  this->U = rst_U;
}

void Arguments::resetRNG(){
  mt19937 newRNG(this->SEED);

  this->rng_tourInit = newRNG;
}