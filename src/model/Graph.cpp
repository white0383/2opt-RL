#include "Graph.h"
#include "Distance.h"
#include "Node.h"
#include <iostream>
#include <algorithm>
#include <limits.h> // INT_MIN INT_MAX

using namespace std;

namespace GraphHelper{
  bool compare(pair<int,double> a, pair<int,double> b){
    if (a.second == b.second) {
      return a.first < b.first;
    } else {
      return a.second < b.second;
    }
  };

  vector<int> setXYMinMax(vector<Node>& nodes){
    int xMin = INT_MAX;
    int xMax = INT_MIN;
    int yMin = INT_MAX;
    int yMax = INT_MIN;

    for(auto node : nodes){
      if(xMin > node.x) xMin = node.x;
      if(xMax < node.x) xMax = node.x;
      if(yMin > node.y) yMin = node.y;
      if(yMax < node.y) yMax = node.y;
    }

    vector<int> rst_vec = {xMin,xMax,yMin,yMax};

    return rst_vec;
  }

  vector<ScaledNode> genScaledNodes(vector<Node>& nodes, int xMin, int xMax, int yMin, int yMax){
    int x_interval = xMax - xMin;
    int y_interval = yMax - yMin;

    vector<ScaledNode> rst_vec;
    rst_vec.reserve(nodes.size());

    double rst_scaledNode_x = 0.0;
    double rst_scaledNode_y = 0.0;

    for(auto node:nodes){
      rst_scaledNode_x = (double)(node.x - xMin) / x_interval;
      rst_scaledNode_y = (double)(node.y - yMin) / y_interval;

      ScaledNode rst_scaleNode = ScaledNode(node.index, rst_scaledNode_x, rst_scaledNode_y);
      rst_vec.emplace_back(rst_scaleNode);
    }

    return rst_vec;
  }
}

Graph::Graph(vector<vector<int>> &node_data){
  this->n = node_data.size();
  this->nodes.reserve(this->n + 2);

  //add dummy node in nodes[0]
  this->nodes.emplace_back(0,0,0);

  for (auto single_data : node_data) {
    this->nodes.emplace_back(single_data[0], single_data[1], single_data[2]);
  }

  //nodes[0] == nodes[n]
  this->nodes[0].setNode(this->nodes[this->n]);
  //nodes[n+1] == nodes[1]
  this->nodes.emplace_back(this->nodes[1]);

  this->setDistOrder();

  vector<int> xyMinMax = GraphHelper::setXYMinMax(this->nodes);
  this->xMin = xyMinMax[0];
  this->xMax = xyMinMax[1];
  this->yMin = xyMinMax[2];
  this->yMax = xyMinMax[3];

  this->scaledNodes = GraphHelper::genScaledNodes(this->nodes, this->xMin, this->xMax, this->yMin, this->yMax);
}

vector<Node> Graph::getNodes() const{
  return this->nodes;
}

vector<ScaledNode> Graph::getScaledNodes() const{
  return this->scaledNodes;
}

Node Graph::getNode(int i) const{
  return this->nodes.at(i);
}

ScaledNode Graph::getScaledNode(int i) const{
  return this->scaledNodes.at(i);
}

int Graph::getN() const {
  return this->n;
}

void Graph::setDistOrder() {
  this->distOrder.reserve(this->n +1);

  pair<int,double> dummyPair(0,0.0);
  vector< pair<int,double> > dummyVector(this->n, dummyPair);
  this->distOrder.emplace_back(dummyVector);

  for(int i=1;i<this->n+1;i++){
    dummyVector.clear();
    for(int j=1;j<this->n+1;j++){
      dummyVector.emplace_back(make_pair(j,dist(this->nodes.at(i),this->nodes.at(j))));
    }
    // sort dummyVector 
    sort(dummyVector.begin(), dummyVector.end(), GraphHelper::compare);
    this->distOrder.emplace_back(dummyVector);
  }
}