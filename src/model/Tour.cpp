#include "Tour.h"
#include "Graph.h"
#include "Distance.h"
#include "Node.h"
#include <vector>
#include <iostream>
#include <cfloat> // DBL_MAX
#include <algorithm> // std::iter_swap

using namespace std;

namespace TourHelper{
  /**
   * Check tour vector is well-defined.
   * If there is any node that apeears more than once in tour vector,
   * or if there is any node that is not included in Graph g,
   * then this tour vector is not well defined
   * 
   * @param 
   * @return true -> this vector is good Tour
   */
  bool verifyTour(const vector<int> &orders, const Graph& g){
    int maxIndex = g.getN();
    vector<bool> visited(maxIndex+1,false);
    bool isWellDefined = true;

    for(int city : orders){
      if((city > maxIndex) || (visited[city] == true)){
        isWellDefined = false;
        break;
      }else{
        visited[city] = true;
      }
    }
    return isWellDefined;
  }

  // when orders is {3,1,2,4}
  // then rst_pi is {4,3,1,2,4,3}
  //  = {orders.front, orders, orders.back}
  vector<int> convertOrdersToPi(const vector<int>& orders){
    vector<int> rst_pi;
    rst_pi.reserve(orders.size() + 2); // rst_pi[0] and rst_pi[n+1]

    rst_pi.emplace_back(orders.back()); // rst_pi[0] = order[n-1]
    for(int order : orders) rst_pi.emplace_back(order); 
    rst_pi.emplace_back(orders.front()); // rst_pi[n+1] = order[0]

    return rst_pi;
  }

  // when orders is {3,1,2,4}
  // then rst_pi_inv is {-1,2,3,1,4}
  // rst_pi_inv[orders[i-1]] = i (i = 1 ~ orders.size)
  vector<int> convertOrdersToPiInv(const vector<int>& orders, int dim){
    vector<int> rst_pi_inv(dim+1,-1);

    for(int i=1; i<=orders.size(); i++){
      rst_pi_inv[orders[i-1]] = i;
    }

    return rst_pi_inv;
  }

  // when pi is {4,3,1,2,4,3}
  // then orders is {3,1,2,4}
  vector<int> convertPiToOrders(vector<int>& pi){
    vector<int> rst_orders;
    rst_orders.reserve(pi.size()-2);

    for(int i = 1; i<=pi.size()-2;i++){
      rst_orders.emplace_back(pi[i]);
    }

    return rst_orders;
  }

} // end namespace tourHelper

Tour::Tour(){
  this->cost = DBL_MAX;
  this->size = 0;
}

Tour::Tour(const vector<int>& orders, const Graph& g){
  if(TourHelper::verifyTour(orders, g) == false){
    cout << "ERROR : improper input vector in Tour object constructor";
    exit(1);
  } else {
    this->cost = DBL_MAX;
    this->size = orders.size();

    this->pi = TourHelper::convertOrdersToPi(orders);
    this->pi_inv = TourHelper::convertOrdersToPiInv(orders, g.getN());
  }
}

void Tour::setNewTour(const vector<int>& newOrders, const Graph& g){
  if(TourHelper::verifyTour(newOrders, g) == false){
    cout << "ERROR : improper input vector in setNewTour method";
    exit(1);
  } else {
    this->cost = DBL_MAX;
    this->size = newOrders.size();

    this->pi.clear();
    this->pi_inv.clear();

    this->pi = TourHelper::convertOrdersToPi(newOrders);
    this->pi_inv = TourHelper::convertOrdersToPiInv(newOrders, g.getN());
  }
}

bool Tour::isCompleteTour(const Graph& g){
  bool isWellDefindeCompleteTour = true;
  vector<int> orders = TourHelper::convertPiToOrders(this->pi);

  if(!(TourHelper::verifyTour(orders, g))){
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

  //add from dist(pi[0]=pi[n],pi[1]) to dist(pi[n-1], pi[n])
  for(it=this->pi.begin(); it!=this->pi.end()-2 ;it++){
    // when g.distinfo is not operated (if needed)
    //Node currentNode = g.nodes[(*it)];
    //Node nextNode = g.nodes[(*(it + 1))];
    //cost += dist(currentNode, nextNode);
    cost += g.distMatrix[*(it)][*(it+1)];
  }

  this->cost = cost;
};

void Tour::setCost(double newCost){
  this->cost = newCost;
}

int Tour::getSize() {
  return this->size;
}

// when this.pi is {4,3,1,2,4,3}
// then print "pi : 3 -> 1 -> 2 -> 4 -> 3"
//            "cost : this->cost"
void Tour::printTour() {
  //1. print tour
  cout << "pi : " ;

  vector<int>::iterator it;
  for(it=this->pi.begin()+1; it!=this->pi.end()-1 ;it++){
    cout << *it << " -> ";
  }
  cout << *(this->pi.end()-1) << endl;

  //2. print cost
  cout << "cost : " ;
  if(this->cost == DBL_MAX){
    cout << "Not yet Calculated" << endl;
  } else {
    cout << this->cost << endl;
  }
}

int Tour::getPi(int order){
  return this->pi[order];
}

vector<int> Tour::getPi(){
  return this->pi;
}

int Tour::getPiInv(int index){
  return this->pi_inv[index];
}

vector<int> Tour::getPiInv(){
  return this->pi_inv;
}

vector<int> Tour::getOrders(){
  return TourHelper::convertPiToOrders(this->pi);
}

Node Tour::getPi_node(int order, const Graph& g){
  return g.nodes[this->pi[order]];
}

void Tour::insert(int newNodeIndex, int i, const Graph& g){
  // check newNodeIndex and i are valid
  if((i<1) || (i > this->size)){
    cout << "ERROR : Tour.insert method\n";
    cout << "improper i inputed\n";
    exit(1);
  } else if((newNodeIndex <1) || (newNodeIndex > g.getN())){
    cout << "ERROR : Tour.insert method\n";
    cout << "improper newNodeIndex inputed\n";
    exit(1);
  }

  // change pi
  if(i == this->size) this->pi[0] = newNodeIndex;
  this->pi.insert(this->pi.begin() + i + 1, newNodeIndex);

  // change pi_inv
  for(int j=i+1;j<=this->size+1;j++){
    this->pi_inv[this->pi[j]] = j;
  }

  // change cost
  this->cost = DBL_MAX;

  // change size
  this->size += 1;
}

void Tour::swap(int i, int j){
  // exception check
  if((i<1) || (i>this->size)){
    cout << "ERROR : Tour.swap invalid i\n";
    cout << "i : " << i << endl;
    exit(1);
  } else if((j<1) || (j>this->size)){
    cout << "ERROR : Tour.swap invalid j\n";
    cout << "j : " << j << endl;
    exit(1);
  }

  iter_swap(this->pi.begin() + i, this->pi.begin() + j);
  this->pi_inv[this->pi[i]] = i;
  this->pi_inv[this->pi[j]] = j;

  if(i == 1) this->pi[this->size + 1] = this->pi[i];
  if(j == this->size) this->pi[0] = this->pi[j];
}