#ifndef TSP_FAST2OPT_H
#define TSP_FAST2OPT_H

#include <map>
#include <utility>

#include "../../../model/Tour.h"
#include "../../../model/Graph.h"

/**
 * Faster 2-opt heuristic.
 * First Improvement method
 * Randomly choose pair of nodes to swap.
 * And use pre-sorted array of closest node and distance (Graph.distOrder)
 * 
 * @param Graph g : TSP itself
 * @param Tour pi : initial solution of TSP 
 * @return Tour solution_star : complete Tour of g
 */
Tour fastTwoOpt(const Graph& g, Tour& pi);

Tour fastTwoOptBestInP(const Graph& g, Tour& pi, double pr);
Tour fastTwoOptBest(const Graph& g, Tour& pi);
Tour fastTwoOptLeast(const Graph& g, Tour& pi);

// for recycle code in LinQ
namespace FastTwoOptHelper{
  bool isAdjacent(int i, int j, int n);
  double getAfterMinusBefore(int p,int pp,int q,int qp, const Graph& g);
  double getAfterMinusBefore(Tour& pi, pair<int,int> IJ, const Graph& g);
  pair<int,int> getMaxScoredIJ(map<double,pair<int,int> >& m);
  void swapTwoOpt(Tour& pi, pair<int,int> IJ);
}

/**
 * only search
 * i in [1,n-2]
 * j in [i+2,n]
 * 
 * speed is almost same to fastTwoOpt
 * but show bad performance in high probability(80~90%)
 * 
 * just for archiving
 */
Tour fastTwoOptBigJ(const Graph& g, Tour& pi);

#endif // TSP_FAST2OPT_H