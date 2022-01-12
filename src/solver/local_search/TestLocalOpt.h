#ifndef TSP_TESTLOCALOPT_H
#define TSP_TESTLOCALOPT_H

#include <ctime>
#include <string>
#include <vector>
#include <utility> // std::pair
#include <fstream>

#include "../../model/Arguments.h"

using namespace std;

class LocalSearchTester;

class Event_mini{
  public:
    int tourCount;
    double initScore;
    double optScore;
    double bestScore;
    double spendSec;
    double swapNum;
  public:
    Event_mini(LocalSearchTester& LStester);
    void print(ofstream& f);
};

class LocalSearchTester{
  public:
    double spendSec;
    time_t startTime;
    double bestScore;
    string LOCAL_METHOD;
    double secLimit;
    double maxInitTour;
    vector<pair<int, double> > bestScoreVec;
    int initTourCount;
    double pr;
    string TERMINATE_CONDITION;
    int swapNum;
    int totalSwapNum;

    double initScore;
    double optScore;
    vector<Event_mini> eventVec;

  public:
    LocalSearchTester(const Arguments& tspArgs, string& LOCAL_METHOD);

    void setPr(double pr);
    bool checkTerminate();
    void run(const Arguments& tspArgs);
};

#endif // TSP_TESTLOCALOPT_H