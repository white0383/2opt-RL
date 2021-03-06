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
  this->maxInitTour = tspArgs.EPI_LIMIT;
  this->bestScoreVec = {};
  this->initTourCount = 0;
  this->pr = -1; // when pr is not activated
  this->TERMINATE_CONDITION = tspArgs.LEARN_TERMINATE_METHOD;

  this->initScore = 0;
  this->optScore = 0;
  this->eventVec.clear();
  this->swapNum = 0;
  this->totalSwapNum = 0;
}

void LocalSearchTester::run(const Arguments& tspArgs){
  while(this->checkTerminate()){
    Tour initTour = generateInitialSolution(tspArgs);
    this->initTourCount += 1;
    initTour.setCost(tspArgs.V);
    this->initScore = initTour.getCost();

    Tour optTour;
    if(this->pr == -1){
      optTour = searchLocalOpt(tspArgs.V, this->LOCAL_METHOD, initTour);
    } else {
      optTour = searchLocalOpt(tspArgs.V, this->LOCAL_METHOD, initTour, this->pr);
    }
    optTour.setCost(tspArgs.V);
    this->optScore = optTour.getCost();

    if(this->bestScore > optTour.getCost()){
      this->bestScore = optTour.getCost();
      this->bestScoreVec.push_back(make_pair(this->initTourCount,this->bestScore));
    }

    this->spendSec = (double)(clock() - this->startTime) / CLOCKS_PER_SEC;

    this->swapNum = optTour.getSwapNum();
    this->totalSwapNum += this->swapNum;

    this->eventVec.emplace_back(Event_mini(*this));
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

bool LocalSearchTester::checkTerminate(){
  if(this->TERMINATE_CONDITION == "SEC"){
    return (this->spendSec < this->secLimit);
  } else if(this->TERMINATE_CONDITION == "TOUR"){
    return (this->initTourCount < this->maxInitTour);    
  }
}

Event_mini::Event_mini(LocalSearchTester& LStester){
  this->tourCount = LStester.initTourCount;
  this->initScore = LStester.initScore;
  this->optScore  = LStester.optScore;
  this->bestScore = LStester.bestScore;
  this->spendSec  = LStester.spendSec;
  this->swapNum   = LStester.swapNum;
}

void Event_mini::print(ofstream& f){
  f << this->tourCount << " ,";
  f << this->swapNum   << " ,";
  f << this->initScore << " ,";
  f << this->optScore  << " ,";
  f << this->bestScore << " ,";
  f << this->spendSec  << "\n";
}