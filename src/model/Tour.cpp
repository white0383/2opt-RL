#include "Tour.h"
#include "Graph.h"
#include "Distance.h"
#include "Node.h"
#include <vector>
#include <iostream>
#include <cfloat> // DBL_MAX

using namespace std;

namespace tourHelper{

} // end namespace tourHelper

Tour::Tour(){
  this->cost = DBL_MAX;
  this->size = 0;
}

Tour::Tour(const vector<int>& order, const Graph& g){
  if(verifyTour(order, g) == false){
    cout << "ERROR : improper input vector in Tour object constructor";
    exit(1);
  } else {
    this->tour = order;
    this->cost = DBL_MAX;
    this->size = order.size();
  }
}

void Tour::setNewTour(const vector<int>& newTour, const Graph& g){
  if(verifyTour(newTour, g) == false){
    cout << "ERROR : improper input vector in setNewTour method";
    exit(1);
  } else {
    this->tour = newTour;
    this->cost = DBL_MAX;
    this->size = newTour.size();
  }
}

bool Tour::isCompleteTour(const Graph& g){
  bool isWellDefindeCompleteTour = true;

  vector<int> order = this->tour;

  if(!(verifyTour(order, g))){
    cout << "WARNING : this is not complete tour (reason : not a tour)" << endl;
    isWellDefindeCompleteTour = false;
  } else if(this->getSize() != g.getN()){
    cout << "WARNING : this is not complete tour (reason : does not visit every cities)" << endl;
    isWellDefindeCompleteTour = false;
  }

  return isWellDefindeCompleteTour;
};

double Tour::getCost() {
  return this->cost;
};

void Tour::setCost(const Graph &g) {
  double cost = 0;
  vector<int>::iterator it;

  //add from dist(1,2) to add(n-1, n)
  for(it=this->tour.begin(); it!=this->tour.end()-1 ;it++){
    Node currentNode = g.nodes[(*it)];
    Node nextNode = g.nodes[(*(it + 1))];
    cost += dist(currentNode, nextNode);
  }
  //add dist(n,1)
  Node firstNode = g.nodes[this->pi(1)];
  Node lastNode = g.nodes[this->pi(this->size)];
  cost += dist(lastNode, firstNode);

  this->cost = cost;
};

vector<int>& Tour::getTour(){
  return this->tour;
};

int Tour::getSize() {
  return this->size;
}

void Tour::printTour() {
  //1. print tour
  cout << "tour : " ;

  vector<int>::iterator it;
  for(it=this->tour.begin(); it!=this->tour.end() ;it++){
    cout << *it << " -> ";
  }
  cout << this->tour[0] << endl;

  //2. print cost
  cout << "cost : " ;
  if(this->cost == DBL_MAX){
    cout << "Not yet Calculated" << endl;
  } else {
    cout << this->cost << endl;
  }
}

int Tour::pi(int order){
  if((order >= 1) && (order <= this->size)){
    return this->tour.at(order-1);
  } else if(order < 1){
    return Tour::pi(order + this->size);
  } else {
    return Tour::pi(order - this->size);
  }
}

Node Tour::pi_node(int order, const Graph& g){
  return g.nodes[Tour::pi(order)];
}

void Tour::setThisIsLocalOpt(){
  this->localOpt = true;
}

bool Tour::isLocalOpt(){
  return this->localOpt;
}

bool verifyTour(const vector<int> &order, const Graph& g){
  int maxIndex = g.getN();
  vector<bool> visited(maxIndex+1,false);
  bool isWellDefined = true;

  for(int city : order){

    if((city > maxIndex) || (visited[city] == true)){
      isWellDefined = false;
      break;
    }else{
      visited[city] = true;
    }
  }

  return isWellDefined;
}