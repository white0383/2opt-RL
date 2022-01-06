/**
 * Description of methods' name
 * 
 * Details in below
 * 
 * 1 2OPT : 2-opt -> search neighborhood in random sequence
 * 2 F2OPT : fast 2-opt -> improved 2-opt suggested by haraguchi prof. 
 * 3 2OPTO : 2-opt order ->search neighborhood in sequence
 * 
 * 4 2OPTB : 2-opt best -> 2OPT with best improvement
 * 5 F2OPTB : fast 2-opt best -> F2OPT with best improvement
 * 6 2OPTOB : 2-opt order best -> 2OPTO with best improvement
 * 
 * 7 2OPTL : 2-opt least -> 2OPT with least improvement
 * 8 F2OPTL : fast 2-opt least -> F2OPT with least improvement
 * 9 2OPTOL : 2-opt order least -> 2OPTO with least improvement
 * 
 * 10 2OPTP : 2-opt p -> do 2OPTB in probability p or 2OPTL
 * 11 F2OPTP : fast 2-opt p -> do F2OPTB in probability p or F2OPTL
 * 12 2OPTOP : 2-opt order p -> do 2OPTOB in probability p or 2OPTOL
 * ----- not yet implemented (written in 2021 11 13 15:15)
 * A 3OPT : 3-opt
 * B LK : Lin–Kernighan method
 */

#include "SearchLocalOpt.h"
#include <iostream>
#include <string>

#include "./method/TwoOpt.h"
#include "./method/FastTwoOpt.h"
#include "./method/TwoOptOrdered.h"
//#include "./method/ThreeOpt.h"
//#include "./method/LinKernighan.h"
using namespace std;

Tour searchLocalOpt(const Graph& g, const string& method, Tour& pi){
  /**
   * There are several ways to avoid if-else statement like below,
   * but this is much easier to read and manage.
   * 
   * Using string in switch statement makes code longer and hard to read.
   * Moreover, switch statement is not that faster than this short if-else chain (in my opinion)
   */
  if(!(pi.isCompleteTour(g))){
    cout << "ERROR : Tour solution is not a complete tour" << endl;
    exit(1);
  }

  if(method.compare("2OPT") == 0) {
    return twoOpt(g, pi);
  }else if (method.compare("2OPTO") == 0){
    return twoOptOrdered(g,pi);
  }else if (method.compare("F2OPT") == 0){
    return fastTwoOpt(g,pi);

  }else if (method.compare("2OPTB") == 0){
    return twoOptBest(g,pi);
  }else if (method.compare("2OPTL") == 0){
    return twoOptLeast(g,pi);

  }else if (method.compare("2OPTOB") == 0){
    return twoOptOrderedBest(g,pi);
  }else if (method.compare("2OPTOL") == 0){
    return twoOptOrderedLeast(g,pi);
  
  }else if (method.compare("F2OPTB") == 0){
    return fastTwoOptBest(g,pi);
  }else if (method.compare("F2OPTL") == 0){
    return fastTwoOptLeast(g,pi);

  //######### BEGIN: NOT YET IMPLEMENTED
  //}else if (method.compare("3OPT") == 0){
  //}else if (method.compare("LK") == 0){
  //######### END: NOT YET IMPLEMENTED
  }else {
    //後でthrow文で書き直す
    cout << "ERROR : invalid Local Search method name" << endl;
    exit(1);
  }
}

Tour searchLocalOpt(const Graph& g, const string& method, Tour& pi, double p){
  /**
   * There are several ways to avoid if-else statement like below,
   * but this is much easier to read and manage.
   * 
   * Using string in switch statement makes code longer and hard to read.
   * Moreover, switch statement is not that faster than this short if-else chain (in my opinion)
   */
  if(!(pi.isCompleteTour(g))){
    cout << "ERROR : Tour solution is not a complete tour" << endl;
    exit(1);
  }
  if((p<0) || (p>1)){
    cout << "ERROR : searchLocalOpt improper p"<< endl;
    cout << "p should be in [0,1] but your p is " << p << endl;
    exit(1);
  }

  Tour rst = Tour();

  if(method.compare("2OPTP") == 0) {
    return twoOptBestInP(g,pi,p);
  }else if (method.compare("2OPTOP") == 0){
    return twoOptOrderedBestInP(g,pi,p);
  }else if (method.compare("F2OPTP") == 0){
    return fastTwoOptBestInP(g,pi,p);
  }else {
    //後でthrow文で書き直す
    cout << "ERROR : invalid Local Search method name" << endl;
    exit(1);
  }
}