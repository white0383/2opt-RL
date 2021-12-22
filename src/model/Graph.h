#ifndef TSP_GRAPH_H
#define TSP_GRAPH_H

#include <vector>
#include <utility>
#include "Node.h"

using namespace std;

class Graph {
  public:
    /**
     * Node set of the graph
     */
    vector<Node> nodes;

    /**
     * the number of node that this graph has
     */
    int n;

    /**
     * Let distOrder[i][j] is (k, r) .
     * then, jth closest node with nodes[i] is nodes[k].
     * and r is dist(nodes[i], nodes[k]).
     * 
     * distOrder[i][0] is always (i,0) 
     * because dist(nodes[i], nodes[i]) is zero.
     * 
     * i is integer from 1 to n.
     * j is integer from 0 to n-1.
     * k is integer from 1 to n.
     * r is positive real number.
     * 
     * This member instance makes 2-opt much faster.
     * 
     * It takes O(n log n) to construct distOrder,
     */
    vector< vector< pair<int, double> > > distOrder;

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
    vector<ScaledNode> scaledNodes;
    int xMin;
    int xMax;
    int yMin;
    int yMax;
  
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
    Graph(vector<vector<int> > &node_data);

    /**
     * Return the node set of this graph.
     */
    vector<Node> getNodes() const;
    vector<ScaledNode> getScaledNodes() const;

    /**
     * Return a node whose index is i.
     */
    Node getNode(int i) const;
    ScaledNode getScaledNode(int i) const;

    /**
     * Return the number of nodes that this graph.
     * 
     * @return the size of "nodes" vector
     */
    int getN() const;

    /**
     * Build distOrder member variable.
     */
    void setDistOrder();
};

#endif //TSP_GRAPH_H