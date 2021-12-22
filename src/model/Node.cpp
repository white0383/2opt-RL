#include "Node.h"
#include <iostream>

using namespace std;

Node::Node(int index, int x, int y) {
  this->index = index;
  this->x = x;
  this->y = y;
}

void Node::printNode() {
  cout << "index : " << this->index << "\t" ;
  cout << "x : " << this->x << "\t";
  cout << "y : " << this->y << endl;
}

void Node::setNode(Node& newNode) {
  this->index = newNode.index;
  this->x = newNode.x;
  this->y = newNode.y;
}

ScaledNode::ScaledNode(int index, double x, double y){
  this->index = index;
  this->x = x;
  this->y = y;
}

void ScaledNode::printScaledNode(){
  cout << "index : " << this->index << "\t" ;
  cout << "x : " << this->x << "\t";
  cout << "y : " << this->y << endl;  
}