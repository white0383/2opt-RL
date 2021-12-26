#include <iostream>
#include <stdlib.h>

using namespace std;

int main(){
  srand(152773);
  int nodeNum = 10;
  for(int i=0; i<nodeNum ; i++){
    int xy = rand() % 169;
    int x =xy / 13;
    int y = xy % 13;
    cout << xy << " " << x << " " << y << endl;
  }
}