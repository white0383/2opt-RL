#ifndef TSP_SCALEDFAST2OPT_H
#define TSP_SCALEDFAST2OPT_H

#include "../../../model/Tour.h"
#include "../../../model/Arguments.h"

/**
 * Faster 2-opt heuristic(scaled)
 * Randomly choose pair of nodes to swap.
 * And use pre-sorted array of closest node and distance (Graph.distOrder)
 * 
 * use scaledNode instead of Node.
 * pass swap when improvement is lesser than eps_improve
 * sum improved when swap occured
 * 
 */
Tour scaledFastTwoOpt(const Arguments& tspArgs, Tour& pi);

#endif // TSP_SCALEDFAST2OPT_H