#include "Graph.h"
#include "Node.h"
#include "Distance.h"
#include "../helper/FileIOHelper.h"
#include <float.h> // DBL_MIN DBL_MAX
#include <algorithm>

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

  bool compare(pair<int,double> a, pair<int,double> b){
    if (a.second == b.second) {
      return a.first < b.first;
    } else {
      return a.second < b.second;
    }
  }

  /**
   * let A is distMatrix
   * let B is distOrder
   * let n is |V|
   * 
   * A[i][j] is dist(nodes[i],nodes[j])
   * i and j are in [0,n+1]
   * 
   * B[i][j] is an index of j-th closest node of nodes[i]
   * i is in [1,n]
   * j is in [0,n-1]
   * B[0] is a dummyVector whose contents are all -1
   */
  void setDistInfos(Graph& g){
    int n = g.getN();
    g.distMatrix.reserve(n+2);
    g.distOrder.reserve(n+1);

    vector<double> distMatrixColumn(n+2,0);
    vector<int> distOrderColumn(n,-1);

    pair<int,double> dummyPair(0,0.0);
    vector< pair<int,double> > dummyVector(n, dummyPair);
    int i=0;
    int j=0;
    double tmp_dist_ij = 0.0;

    // i = 0
    for(j=0;j<=n+1;j++){
      distMatrixColumn[j] = dist(g.nodes[i], g.nodes[j]);
    }
    g.distMatrix.emplace_back(distMatrixColumn);
    g.distOrder.emplace_back(distOrderColumn); // all -1

    // i = 1 ~ n
    for(i=1;i<=n;i++){
      // j = 0
      distMatrixColumn[0] = dist(g.nodes[i], g.nodes[0]);

      // j = 1 ~ n
      dummyVector.clear();
      for(j=1;j<=n;j++){
        tmp_dist_ij = dist(g.nodes[i],g.nodes[j]);
        distMatrixColumn[j] = tmp_dist_ij;
        dummyVector.emplace_back(make_pair(j,tmp_dist_ij));
      }
      sort(dummyVector.begin(),dummyVector.end(),GraphHelper::compare);
      for(int k=0;k<n;k++){
        distOrderColumn[k] = dummyVector[k].first;
      }

      // j = n+1
      distMatrixColumn[n+1] = dist(g.nodes[i], g.nodes[n+1]);

      // push back
      g.distMatrix.emplace_back(distMatrixColumn);
      g.distOrder.emplace_back(distOrderColumn);
    }

    // i = n+1
    i = n+1;
    for(j=0;j<=n+1;j++){
      distMatrixColumn[j] = dist(g.nodes[i], g.nodes[j]);
    }
    g.distMatrix.emplace_back(distMatrixColumn);

  }
} // end namespace GraphHelper

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

  // set distOrder and distMatrix
  GraphHelper::setDistInfos(*(this));
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