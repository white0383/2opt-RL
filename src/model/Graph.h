#ifndef TSP_GRAPH_H
#define TSP_GRAPH_H

#include <vector>
#include <string>
#include "Node.h"

using namespace std;

class Graph {
  public:
    /**
     * Node set of the graph
     * from original TSP instance
     */
    vector<Node> nodes_ori;
    /**
     * vector of MinMax Scaled Nodes
     * 
     * let Nodes = {(x_i, y_i)} (i = 1 ~ n)
     * let xMin(resp. Max) = min(resp. max){x_i} 
     * let yMin(resp. Max) = min(resp. max){y_i}
     * 
     * let scaled_x_i = (x_i - xMin) / (xMax - xMin)
     * let scaled_y_i = (y_i - yMin) / (yMax - yMin)
     * 
     * then, scaledNodes := {(scaled_x_i, scaled_y_i)} (i = 1~n)
     */
    vector<Node> nodes;

    /**
     * the number of node that this graph has
     */
    int n;
  
  public:
    /**
     * Default Graph Constructor
     * Create an empty Graph object
     */
    Graph() = default;

    /**
     * Graph Constructor
     * Create Graph object with node data.
     * 
     * @param |V| * 3 int 2D array node_data
     */
    Graph(string& instance_name);

    /**
     * Return the node set of this graph.
     */
    vector<Node> getNodes() const;

    /**
     * Return a node whose index is i.
     */
    Node getNode(int i) const;

    /**
     * Return the number of nodes that this graph.
     * 
     * @return the size of "nodes" vector
     */ 
    int getN() const;
};

#endif //TSP_GRAPH_H