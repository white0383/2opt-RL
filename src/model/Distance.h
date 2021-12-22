#ifndef TSP_DISTANCE_H
#define TSP_DISTANCE_H

#include "Node.h"
#include "Tour.h"
#include "Graph.h"

double dist(const Node& n1, const Node& n2);

double dist(const ScaledNode& sn1, const ScaledNode& sn2);

int distSquare(const Node& n1, const Node& n2);

#endif //TSP_DISTANCE_H