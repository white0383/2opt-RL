#ifndef TSP_RANDOMTOUR_H
#define TSP_RANDOMTOUR_H

#include "../../../model/Tour.h"
#include "../../../model/Graph.h"

/**
 * 
 * @param Graph g : TSP itself
 * @return Tour solution : complete Tour of g
 */
Tour randomTour(const Graph& g);

#endif //TSP_RANDOMTOUR_H