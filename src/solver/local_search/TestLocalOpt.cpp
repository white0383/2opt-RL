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
  this->pr = -1; // when pr is not activated
}

void LocalSearchTester::run(const Arguments& tspArgs){
  int ite = 1;

  while(this->spendSec < this->secLimit){
    Tour initTour = generateInitialSolution(tspArgs);
    this->initTourCount += 1;
    initTour.setCost(tspArgs.V);
    Tour optTour;
    if(this->pr == -1){
      optTour = searchLocalOpt(tspArgs.V, this->LOCAL_METHOD, initTour);
    } else {
      optTour = searchLocalOpt(tspArgs.V, this->LOCAL_METHOD, initTour, this->pr);
    }
    optTour.setCost(tspArgs.V);

    if(this->bestScore > optTour.getCost()){
      this->bestScore = optTour.getCost();
      this->bestScoreVec.push_back(make_pair(ite,this->bestScore));
    }

    ite++;
    this->spendSec = (double)(clock() - this->startTime) / CLOCKS_PER_SEC;
  }
}

void LocalSearchTester::setPr(double pr){
  if((pr >= 0) && (pr <= 1)){
    this->pr = pr;
  } else {
    cout << "ERROR : LocalSearchTester setPr improper input p\n";
    cout << "p should be in [0,1]" << endl;
    cout << "yout p : " << pr << endl;
    exit(1);     
  }
}