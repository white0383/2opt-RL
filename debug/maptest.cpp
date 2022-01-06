#include <iostream>
#include <map>
#include <string>
#include <utility> // std::pair
#include <ctime> 

using namespace std;

void echo(map<int,string>& mp){
  for(auto m:mp){
    cout << m.first << " -> " << m.second << endl;
  }
}

void echo(map<double,string>& mp){
  for(auto m:mp){
    cout << m.first << " -> " << m.second << endl;
  }  
}

void echo(map<double, pair<int,int> >& mp){
  for(auto m:mp){
    cout << m.first << " -> (" << m.second.first << ", " << m.second.second << ")" << endl;
  }
}

int main(){
  map<int, string> testMap;

  testMap[3] = "coffee";
  testMap[2] = "tea";
  testMap[-1] = "ice";
  //testMap.insert({3,"black coffee"});
  //testMap.emplace(3,"black coffee");
  testMap[3] = "black coffee";
  testMap[2] = "red tea";

  echo(testMap);

  map<double,string> animalMap;
  animalMap[0.4] = "dog";
  animalMap[0.1] = "cat";
  animalMap[1.2] = "horse";
  animalMap[0.4] = "loofy";
  
  echo(animalMap);

  map<double, pair<int,int> > swapMap;
  swapMap[0.9] = make_pair(3,10);
  swapMap[0.7] = make_pair(4,-1);
  swapMap[1.8] = make_pair(7,1);
  swapMap[0.9] = make_pair(2,90);

  echo(swapMap);  

  map<double, pair<int,int> > stressMap;
  for(int i=0;i<500000;i++){
    stressMap[(double)i] = make_pair(i,i+i);
  }

  time_t startT = clock();
  auto tmp_begin = stressMap.begin()->first;
  double spent_begin = (double)(clock() - startT) / CLOCKS_PER_SEC;

  startT = clock();
  auto tmp_rbegin = stressMap.rbegin()->first;
  double spent_rbegin = (double)(clock() - startT) / CLOCKS_PER_SEC;

  cout << spent_begin << " " << spent_rbegin << endl;
  return 0;
}