/**
 * Description of methods' name
 * 
 * Details in below
 *  11 Animated Algorithms for the Traveling Salesman Problem, Lawrence Weru, https://stemlounge.com/animated-algorithms-for-the-traveling-salesman-problem/
 *  巡回セールスマン問題への招待, 4,5章, 山本 and 久保, 1997
 *  In pursuit of the Traveling Salesman : Mathematics at the Limits of Computation Account, Chapter 4, William J.Cook, 2012
 *  Lecture node based on upper book, https://www.coga.tu-berlin.de/fileadmin/i26/download/AG_DiskAlg/FG_KombOptGraphAlg/teaching/adm2ss14/lectures/lecture.18.2014-06-19.pdf
 * 
 * 1 RT : Random Tour
 * 2 NN : Nearest Neighbor method
 * 6 FI : Farthest Insertion method
 * 
 * ----- not yet implemented (written in 2021 10 05 14:53)
 * 3 NA : Nearest Addition method (prim method like)
 * 4 NI : Nearest Insertion method
 * 5 CI : Cheapest Insertion method
 * 7 RI : Random Insertion method
 * 8 GA : Greedy Algorithm (kuskal method like)
 * 9 CW : Clarke-Wright method (saving method)
 * 10 CF : ChristoFides algorithm
 * 11 CH : Convex Hull insertion
 */

#include "GenerateInitialSolution.h"
#include <iostream>
#include <string>

#include "./method/RandomTour.h"
#include "./method/NearestNeighbor.h"
//#include "./method/NearestAddition.h"
//#include "./method/NearestInsertion.h"
//#include "./method/CheapestInsertion.h"
#include "./method/FarthestInsertion.h"
//#include "./method/RandomInsertion.h"
//#include "./method/GreedyAlgorithm.h"
//#include "./method/ClarkeWright.h"
//#include "./method/ChristofidesAlgorithm.h"
//#include "./method/ConvexHullInsertion.h"
using namespace std;

Tour generateInitialSolution(const Graph& g, const string& method){
  /**
   * There are several ways to avoid if-else statement like below,
   * but this is much easier to read and manage.
   * 
   * Using string in switch statement makes code longer and hard to read.
   * Moreover, switch statement is not that faster than this short if-else chain (in my opinion)
   */
  
  if(method.compare("RT") == 0) {
    return randomTour(g);
  }else if (method.compare("NN") == 0){
    return nearestNeighbor(g);
  //######### BEGIN: NOT YET IMPLEMENTED
  //}else if (method.compare("NA") == 0){

  //}else if (method.compare("NI") == 0){

  //}else if (method.compare("CI") == 0){

  }else if (method.compare("FI") == 0){
    return farthestInsertion(g);
  //}else if (method.compare("RI") == 0){

  //}else if (method.compare("GA") == 0){

  //}else if (method.compare("CW") == 0){

  //}else if (method.compare("CF") == 0){

  //}else if (method.compare("CH") == 0){
  //######### END: NOT YET IMPLEMENTED
  }else {
    //後でthrow文で書き直す
    cout << "ERROR : invalid GenInit method name" << endl;
    exit(1);
  }
}

Tour generateInitialSolution(const Arguments& tspArgs){
    /**
   * There are several ways to avoid if-else statement like below,
   * but this is much easier to read and manage.
   * 
   * Using string in switch statement makes code longer and hard to read.
   * Moreover, switch statement is not that faster than this short if-else chain (in my opinion)
   */
  string method = tspArgs.INIT_SOLUTION_METHOD;
    
  if(method.compare("RT") == 0) {
    return randomTour(tspArgs.V);
  }else if (method.compare("NN") == 0){
    return nearestNeighbor(tspArgs.V);
  //######### BEGIN: NOT YET IMPLEMENTED
  //}else if (method.compare("NA") == 0){

  //}else if (method.compare("NI") == 0){

  //}else if (method.compare("CI") == 0){

  }else if (method.compare("FI") == 0){
    return farthestInsertion(tspArgs.V);
  //}else if (method.compare("RI") == 0){

  //}else if (method.compare("GA") == 0){

  //}else if (method.compare("CW") == 0){

  //}else if (method.compare("CF") == 0){

  //}else if (method.compare("CH") == 0){
  //######### END: NOT YET IMPLEMENTED
  }else {
    //後でthrow文で書き直す
    cout << "ERROR : invalid GenInit method name" << endl;
    exit(1);
  }
}
