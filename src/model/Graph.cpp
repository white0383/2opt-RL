#include "Graph.h"
#include "Node.h"
#include "../helper/FileIOHelper.h"
#include <float.h> // DBL_MIN DBL_MAX

using namespace std;

namespace GraphHelper{
  vector<Node> scaleNodes(vector<Node>& nodes_ori){
    // find minimum and maximum coordinates
    double xMin = DBL_MAX;
    double xMax = DBL_MIN;
    double yMin = DBL_MAX;
    double yMax = DBL_MIN;

    for(auto node : nodes_ori){
      if(xMin > node.x) xMin = node.x;
      if(xMax < node.x) xMax = node.x;
      if(yMin > node.y) yMin = node.y;
      if(yMax < node.y) yMax = node.y;
    }

    // Min-Max scale nodes_ori
    vector<Node> rst_vec;
    rst_vec.reserve(nodes_ori.size());

    double rst_scaled_x = 0.0;
    double rst_scaled_y = 0.0;

    for(auto node : nodes_ori){
      rst_scaled_x = (node.x - xMin) / (xMax - xMin);
      rst_scaled_y = (node.y - yMin) / (yMax - yMin);
      rst_vec.emplace_back(node.index, rst_scaled_x, rst_scaled_y);
    }

    return rst_vec;
  }
}

Graph::Graph(string& instance_name){
  // read TSP instance data file
  vector<vector<double> > nodes_data = readTSPFile(instance_name);
  this->n = nodes_data.size();

  // copy TSP instance to nodes_ori
  this->nodes_ori.reserve(this->n + 2); // pi(0) and pi(n+1)
  this->nodes_ori.emplace_back(0,0.0,0.0); // dummy data in pi(0)
  for(auto node_data : nodes_data){
    this->nodes_ori.emplace_back((int)node_data[0],node_data[1],node_data[2]);
  }
  this->nodes_ori[0].setNode(this->nodes_ori[this->n]); // nodes_ori[0] == nodes_ori[n]
  this->nodes_ori.emplace_back(this->nodes_ori[1]); // nodes_ori[n+1] == nodes_ori[1];

  // nodes is Min-Max scald nodes_ori
  this->nodes = GraphHelper::scaleNodes(this->nodes_ori);
}

vector<Node> Graph::getNodes() const{
  return this->nodes;
}

Node Graph::getNode(int i) const{
  return this->nodes.at(i);
}

int Graph::getN() const {
  return this->n;
}

/*
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
*/