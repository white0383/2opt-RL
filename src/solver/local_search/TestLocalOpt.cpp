#include "./TestLocalOpt.h"
#include "./SearchLocalOpt.h"
#include "../initial_solution/GenerateInitialSolution.h"
#include "../../model/Tour.h"
#include <float.h>
#include <iostream>

LocalSearchTester::LocalSearchTester(const Arguments& tspArgs, string& _LOCAL_METHOD){
  this->spendSec = 0;
  this->startTime = clock();
  this->bestScore = DBL_MAX;
  this->LOCAL_METHOD = _LOCAL_METHOD;
  this->secLimit = tspArgs.SEC_LIMIT;
  this->bestScoreVec = {};
  this->initTourCount = 0;
}

void LocalSearchTester::run(const Arguments& tspArgs){
  int ite = 1;

  while(this->spendSec < this->secLimit){
    Tour initTour = generateInitialSolution(tspArgs);
    this->initTourCount += 1;
    initTour.setCost(tspArgs.V);
    Tour optTour = searchLocalOpt(tspArgs.V, this->LOCAL_METHOD, initTour);
    optTour.setCost(tspArgs.V);

    if(this->bestScore > optTour.getCost()){
      this->bestScore = optTour.getCost();
      this->bestScoreVec.push_back(make_pair(ite,this->bestScore));
    }

    ite++;
    this->spendSec = (double)(clock() - this->startTime) / CLOCKS_PER_SEC;
  }
}