#include <vector>
#include <random>
#include <numeric> // std::iota
#include <algorithm> // std::shuffle
#include <iostream> // std::cout
#include <set>
#include "./mt19937ar.h"
#include "./RandVec.h"

using namespace std;

vector<int> genRandDiffIntVec(int min, int max, int num){
  //Exception check
  if(min >= max){
    cout << "ERROR : genRandDiffIntVec : min is greater then max" << endl;
    exit(1);
  } else if((max - min + 1) < num){
    cout << "ERROR : genRandDiffIntVec : num is greater than (max - min + 1)" << endl;
    exit(1);
  }

  vector<int> tmpVec(max - min + 1);
  iota(tmpVec.begin(),tmpVec.end(), min);
  mt19937 mtRand(genrand_int32());
  shuffle(tmpVec.begin(),tmpVec.end(),mtRand);

  vector<int> rstVec;
  rstVec.reserve(num);
  for(int i=0;i<num;i++){
    rstVec.emplace_back(tmpVec[i]);
  }

  return rstVec;
}

vector<int> genRandDiffIntVec(int n, int m){
  return genRandDiffIntVec(1,n,m);
}

vector<int> genRandDiffIntVecBySet(int min, int max, int num){
  //Exception check
  if(min >= max){
    cout << "ERROR : genRandDiffIntVecBySet : min is greater then max" << endl;
    exit(1);
  } else if((max - min + 1) < num){
    cout << "ERROR : genRandDiffIntVecBySet : num is greater than (max - min + 1)" << endl;
    exit(1);
  }

  vector<int> rstVec;
  rstVec.reserve(num);
  int ele = 0;
  int interval = max - min + 1;
  set<int> eleSet;

  while(rstVec.size() < num){
    ele = (genrand_int32() % interval ) + min;
    bool isNotDuplicated = eleSet.insert(ele).second;
    if(isNotDuplicated){
      rstVec.emplace_back(ele);
    }
  }

  return rstVec;
}

vector<int> genRandDiffIntVecBySet(int n, int m){
  return genRandDiffIntVecBySet(1,n,m);
}

vector<int> genRandDiffIntVecWithExceptionSec(int min, int max, int num, set<int> exceptionSet){
  //Exception check
  if(min >= max){
    cout << "ERROR : genRandDiffIntVecBySet : min is greater then max" << endl;
    exit(1);
  } else if((max - min + 1 - exceptionSet.size()) < num){
    cout << "ERROR : genRandDiffIntVecBySet : num is greater than (max - min + 1 - |exceptionSet|)" << endl;
    exit(1);
  }

  vector<int> rstVec;
  rstVec.reserve(num);
  int ele = 0;
  int interval = max - min + 1;

  while(rstVec.size() < num){
    ele = (genrand_int32() % interval ) + min;
    bool isNotDuplicated = exceptionSet.insert(ele).second;
    if(isNotDuplicated){
      rstVec.emplace_back(ele);
    }
  }
  return rstVec;
}

vector<int> genRandDiffIntVecWithExceptionSec(int n, int m, set<int> exceptionSet){
  return genRandDiffIntVecWithExceptionSec(1,n,m,exceptionSet);
}