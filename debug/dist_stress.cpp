#include <iostream>
#include <ctime>
#include <vector>
#include <random>
#include <limits.h>
#include <math.h>

using namespace std;

int main(){
  int experi = 4;

  vector<double> times(experi);
  srand(1234);

  int n = 10;
  for(int i=0;i<experi;i++){
    time_t start = clock();

    n = n*10;
    vector<double> vec(n);
    vector<vector<double> > dist;
    for(int j=0;j<vec.size();j++){
      vec[j] = (double)(rand())/INT_MAX;
    }
    for(int j=0;j<vec.size();j++){
      vector<double> tmp(n);
      for(int k=0;k<vec.size();k++){
        tmp[k]= vec[j] * vec[k];
      }
      dist.emplace_back(tmp);
    }

    double t = (double)(clock() - start) / CLOCKS_PER_SEC;
    times[i]=t;

    cout << n << endl;
  }

  for(double foo:times) cout << foo << " ";

  return 0;
}