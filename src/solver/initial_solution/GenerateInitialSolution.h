#ifndef TSP_GENINITSOLUTION_H
#define TSP_GENINITSOLUTION_H

#include <string>
#include <random>
#include "../../model/Tour.h"
#include "../../model/Graph.h"
#include "../../model/Arguments.h"

/**
 * Return initial solution of TSP as Tour class
 * 
 * @param Graph g : TSP itself
 * @param string method : the name of init-solution generate method
 * @return Tour : initial solution of TSP
 */
Tour generateInitialSolution(const Graph& g, const string& method);
Tour generateInitialSolution(const Graph& g, const string& method, mt19937& rng);
Tour generateInitialSolution(const Arguments& tspArgs, mt19937& rng); // with rng

#endif // TSP_GENINITSOLUTION_H