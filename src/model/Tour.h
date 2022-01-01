#ifndef TSP_TOUR_H
#define TSP_TOUR_H

#include <vector>
#include "Graph.h"
#include "Distance.h"
#include "Node.h"

/**
 * Tour and Graph are implemented independently
 * you should use them together when calculate something
 * 
 * Not only a tour which has every city of TSP,
 * but also a subtour which partly has some cities of TSP
 * are Tour object
 * 
 * Example)
 * Graph = {1,2,3,4}  (index of nodes)
 * Tour pi_1 = {4,1,3,2,4,1} -> this is complete tour
 * Tour pi_2 = {2,4,3,2,4} -> this is not complete tour
 * Tour pi_3 = {1,3,2,6} -> this is not Tour
 */

class Tour{
  private:
    /**
     * The order of visiting city
     * integer in the vector represent the index of city.
     */
    vector<int> tour;

    vector<int> pi; // pi[0] = pi[size], pi[size+1]=pi[1]
    vector<int> pi_inv;

    /**
     * Total distance of Tour
     * Sum of every edge on Tour
     */
    double cost;

    /**
     * The number of cities which Tour object has.
     */
    int size;

    /**
     * This is "true" when Tour is local Optimal solution.
     * default value is "false"
     */
    bool localOpt = false;

  public:
    /**
     * Default constructor
     * set cost to DBL_MAX
     * size to zero
     */
    Tour();

    /**
     * Create new Tour object with order vector
     * 
     * Example)
     * Tour t1 = new Tour({1,4,2})
     * t1.tour -> {1,4,2}
     */
    Tour(const vector<int> &order, const Graph& g);

    /**
     * Default destructor
     */
    ~Tour() = default;

    /**
     * Change tour to newOrder
     * reset cost to DBL_MAX
     * set size to newOrder.size()
     * 
     * verify Tour before return
     */
    void setNewTour(const vector<int> &newTour, const Graph& g);

    /**
     * CompleteTour is "Verified" Tour which has g.getN() elements in tour
     * 
     * @param Graph g
     * @return true -> this object is complete tour
     */
    bool isCompleteTour(const Graph& g);

    /**
     * Get Tour's cost
     * 
     * @return this->cost
     */
    double getCost();

    /**
     * Calculate Tour's cost
     * 
     * @param Graph g
     */
    void setCost(const Graph &g);

    /**
     * Get tour vector of this object
     * 
     * @return this->tour 
     */
    vector<int>& getTour();

    /**
     * Get size of this tour object
     * 
     * @return this->size
     */
    int getSize();

    /**
     * Print tour and cost.
     * If cost is not yet calculated (i.e. this->cost == DBL_MAX),
     * then print cost as "Not Calculated"
     * 
     * Example)
     * tour = {4,1,3,2}
     * cost = 102.4
     * 
     * then printTour() is
     * "tour : 4 -> 1 -> 3 -> 2 -> 4
     * cost : 102.4"
     */
    void printTour();

    /**
     * Return the "order"th visited city's index
     * Getter of tour instance
     * 
     * Example)
     * tour = {9,4,3,6}
     * 
     * pi(2) == 4 : Second city of tour is "4"
     * pi(4) == 6 : Fourth city of tour is "6"
     * pi(7) == 3 : Seventh city of tour is same to third city because tour is cycle.
     */
    int pi(int order);

    /**
     * Return the "order"th visited city as Node
     * 
     * Example)
     * tour = {9,4,3,6}
     * 
     * pi_node(2) == g.nodes[4] : Second city of tour is "4"
     * pi_node(4) == g.nodes[6] : Fourth city of tour is "6"
     * pi_node(7) == g.nodes[3] : Seventh city of tour is same to third city because tour is cycle.
     */
    Node pi_node(int order, const Graph& g);

    /**
     * This function is only operated in local searchers
     */
    void setThisIsLocalOpt();

    /**
     * Getter of this->localOpt
     */
    bool isLocalOpt();

};

/**
 * Check tour vector is well-defined.
 * If there is any node that apeears more than once in tour vector,
 * or if there is any node that is not included in Graph g,
 * then this tour vector is not well defined
 * 
 * @param 
 * @return true -> this vector is good Tour
 */
bool verifyTour(const vector<int> &tour, const Graph& g);

#endif //TSP_TOUR_H