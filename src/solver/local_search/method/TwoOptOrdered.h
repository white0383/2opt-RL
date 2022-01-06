#ifndef TSP_2OPTORDERED_H
#define TSP_2OPTORDERED_H

#include "../../../model/Tour.h"
#include "../../../model/Graph.h"

/**
 * Using 2-opt method to find local opt solution.
 * 
 * reference below link
 *  https://en.wikipedia.org/wiki/2-opt
 *  https://stackoverflow.com/questions/53275314/2-opt-algorithm-to-solve-the-travelling-salesman-problem-in-python
 * 
 * @param Graph g : TSP itself
 * @param Tour pi : initial solution of TSP 
 * @return Tour solution_star : complete Tour of g
 */
Tour twoOptOrdered(const Graph& g, Tour& pi);

Tour twoOptOrderedBestInP(const Graph& g, Tour& pi, double p);
Tour twoOptOrderedBest(const Graph& g, Tour& pi);
Tour twoOptOrderedLeast(const Graph& g, Tour& pi);

#endif // TSP_2OPTORDERED_H