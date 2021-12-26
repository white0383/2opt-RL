#ifndef TSP_NODE_H
#define TSP_NODE_H

class Node {
  public:
    int index = 0;
    double x = 0;
    double y = 0;

  public:
    Node() = default;

    Node(int index, double x, double y);

    void printNode();

    void setNode(Node& newNode);
};

#endif //TSP_NODE_H