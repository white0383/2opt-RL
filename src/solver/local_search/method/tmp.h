#ifndef TSP_FAST2OPTTMP_H
#define TSP_FAST2OPTTMP_H

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
Tour fastTwoOptTmp(const Graph& g, Tour& pi);

#endif // TSP_FAST2OPTTMP_H