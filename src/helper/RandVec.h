#include <vector>
#include <set>

using namespace std;

// Generate "num" random different elements in [min, max] as vector
vector<int> genRandDiffIntVec(int min, int max, int num);
// Generate vector has random m different members in [1,n]
vector<int> genRandDiffIntVec(int n, int m);

/**
 * Generate "num" random different elements in [min, max] as vector
 * It is used when (max - min + 1) is very very bigger than num
 * example) num = 10, min = 1, max = 100000000
 * 
 * Use std::set to store rstvec's elements
 * generate random integral in [min, max] , 
 * and store it in std::set
 */
vector<int> genRandDiffIntVecBySet(int min, int max, int num);
vector<int> genRandDiffIntVecBySet(int n, int m);

/**
 * Generate num-dimentional vector whose elements are diffrent each other
 * and randomly choosed in [min, max] - exceptionSet
 */
vector<int> genRandDiffIntVecWithExceptionSec(int min, int max, int num, set<int> exceptionSet);
vector<int> genRandDiffIntVecWithExceptionSec(int n, int m, set<int> exceptionSet);