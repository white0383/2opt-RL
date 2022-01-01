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
    vector<int> pi; // pi[0] = pi[size], pi[size+1]=pi[1]

    // when pi_inv[i] == -1, 
    // it means that a node "i" is not included in this Tour
    // Likewise, when pi_inv[j] > 0
    // it means that a node "j" is included in this Tour
    vector<int> pi_inv; // pi_inv[0] = -1, pi_inv[pi[i]] = i (i = 1~size)

    /**
     * Total distance of this Tour
     * Sum of every edge on Tour
     */
    double cost;

    /**
     * The number of cities which Tour object has.
     */
    int size;

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
     * t1.pi -> {1,4,2}
     */
    Tour(const vector<int> &orders, const Graph& g);

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
    void setNewTour(const vector<int> &newOrders, const Graph& g);

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
    int getPi(int order);
    vector<int> getPi();

    int getPiInv(int index);
    vector<int> getPiInv();

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
    Node getPi_node(int order, const Graph& g);

    /**
     * insert new Node in Tour
     * let index of new node is k
     * let size of tour is m
     * then new Tour is {pi_0 == pi_m, pi_1, ... pi_i, k, pi_i+1, pi_m, pi_m+1 == pi_1}
     * exceptly, when k is equal to m
     * then new Tour is {k, pi_1, ..., pi_m, k, pi_m+1 == pi_1}
     */

    void insert(int newNodeIndex, int i, const Graph& g);
};

#endif //TSP_TOUR_H