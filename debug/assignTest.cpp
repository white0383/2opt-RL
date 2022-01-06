#include <vector>
#include <iostream>

using namespace std;

void ECHO(vector<int> x){
  for(auto a : x){
    cout << a << " ";
  }
  cout << endl;
}

int main(){
  vector<int> t = {1,2,3,4,5};
  ECHO(t);

  t.assign(10,5);
  ECHO(t);

  return 0;
}