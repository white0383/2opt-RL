#ifndef TSP_TESTLOCALOPT_H
#define TSP_TESTLOCALOPT_H

#include <ctime>
#include <string>
#include <vector>
#include <utility> // std::pair

#include "../../model/Arguments.h"

using namespace std;

class LocalSearchTester{
  public:
    double spendSec;
    time_t startTime;
    double bestScore;
    string LOCAL_METHOD;
    double secLimit;
    vector<pair<int, double> > bestScoreVec;
    int initTourCount;
    double pr;

  public:
    LocalSearchTester(const Arguments& tspArgs, string& LOCAL_METHOD);

    void setPr(double pr);
    void run(const Arguments& tspArgs);
};

#endif // TSP_TESTLOCALOPT_H