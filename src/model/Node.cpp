#include "Node.h"
#include <iostream>

using namespace std;

Node::Node(int index, double x, double y) {
  this->index = index;
  this->x = x;
  this->y = y;
}

void Node::printNode() const{
  cout << "index : " << this->index << "\t" ;
  cout << "x : " << this->x << "\t";
  cout << "y : " << this->y << "\n";
}

void Node::setNode(Node& newNode) {
  this->index = newNode.index;
  this->x = newNode.x;
  this->y = newNode.y;
}