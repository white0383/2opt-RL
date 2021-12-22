#ifndef TSP_FARTHESTINSERTION_H
#define TSP_FARTHESTINSERTION_H

#include "../../../model/Tour.h"
#include "../../../model/Graph.h"

/**
 * 
 * @param Graph g : TSP itself
 * @return Tour solution : complete Tour of g
 */
Tour farthestInsertion(const Graph& g);

#endif // TSP_FARTHESTINSERTION_H