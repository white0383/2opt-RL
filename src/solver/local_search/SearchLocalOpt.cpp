/**
 * Description of methods' name
 * 
 * Details in below
 * 
 * 1 2OPT : 2-opt -> search neighborhood in sequence
 * 2 F2OPT : fast 2-opt -> improved 2-opt suggested by haraguchi. prof 
 * ----- not yet implemented (written in 2021 11 13 15:15)
 * 3 3OPT : 3-opt
 * 4 LK : Lin–Kernighan method
 */

#include "SearchLocalOpt.h"
#include <iostream>
#include <string>

#include "./method/TwoOpt.h"
#include "./method/FastTwoOpt.h"
//#include "./method/RandomTwoOpt.h"
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

  Tour rst = Tour();

  if(method.compare("2OPT") == 0) {
    return twoOpt(g, pi);
  }else if (method.compare("F2OPT") == 0){
    return fastTwoOpt(g,pi);
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