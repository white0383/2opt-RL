#ifndef TSP_RANDOMTOUR_H
#define TSP_RANDOMTOUR_H

#include "../../../model/Tour.h"
#include "../../../model/Graph.h"
#include <random>

/**
 * 
 * @param Graph g : TSP itself
 * @return Tour solution : complete Tour of g
 */
Tour randomTour(const Graph& g);
Tour randomTour(const Graph& g, mt19937& rng);

#endif //TSP_RANDOMTOUR_H