#ifndef TSP_NEARESTNIEGHBOR_H
#define TSP_NEARESTNIEGHBOR_H

#include "../../../model/Tour.h"
#include "../../../model/Graph.h"

/**
 * 
 * @param Graph g : TSP itself
 * @return vector<int> order : the order to visit cities
 */
Tour nearestNeighbor(const Graph& g);

#endif //TSP_NEARESTNIEGHBOR_H