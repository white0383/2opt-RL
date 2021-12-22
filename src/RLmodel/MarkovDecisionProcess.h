#ifndef TSP_TMPMDP_H
#define TSP_TMPMDP_H

#include <vector>
#include <utility> //std::pair
#include <cfloat> // DBL_MAX

#include "../model/Tour.h"
#include "../model/Arguments.h"

//predeclaration
class State;
class Action;
class MDP;
class LinearFittedQIteration;
class DataSet;

using namespace std;

namespace StateHelper{
  /**
   * initialization of vector pi_vec and pi_inv_vec
   * Convert pi_tour to 2 vectors
   * pi_vec and pi_inv_vec have to be declared before use this
   * They are cleared to empty and redifined by this
   * This mainly used in perturbation
   * 
   * description of arguments)
   * 1. Tour& pi_tour : complete tour of TSP
   * 2. pi_vec : map order to node
   * 3. pi_inv_vec : map node to order
   * 
   * example)
   * Tour.tour = {1,4,2,3}, n=4
   * then, 
   * pi_vec = {3,1,4,2,3,1} 
   * pi_vec[0] is pi_vec[n]
   * pi_vec[n+1] is pi_vec[1]
   * pi_inv_vec = {0,1,3,4,2}
   * pi_inv_vec[0] is always 0 (dummy data)
   * pi_vec.at(pi_inv_vec.at(i)) == i 
   * pi_inv_vec.at(pi_vec.at(j)) == j
   * 
   * In Tour class, 
   * method called .pi and .piInverse are already implemented
   * but .piInverse is slow ( O(n) )
   * 
   * In perturbation, 
   * Tour of TSP is handled as pi_vec and pi_inv_vec internaly
   * Like 2OPT and F2OPT, pi_vec finally re-converted to Tour
   * In that time, pi_vec[0] and pi_vec[n+1] have to be cut
   */
  void initPiAndPiInv(Tour& pi_tour, vector<int>& pi_vec, vector<int>& pi_inv_vec);

  //do the same thing with initPiAndPiInv, but pi_inv only
  void initPiInv(Tour& pi_tour, vector<int>& pi_inv_vec);

  void eraseDummiesInPiVec(vector<int>& pi_vec);
}

namespace ActionHelper{
  /**
   * encode a pair (a,b) to int x
   * n is maximum value of b (i.e. the number of cities)
   * a < b is always satisfied
   * x is between 1 and (n*n - n) / 2 (i.e. summation of from 1 to n-1)
   * 
   * example) 
   * (1,2) -> 1 , (1,3) -> 2, ... , (1,n) -> n-1, 
   * (2,3) -> (n-1) + 1, (2,4) -> (n-1) + 2 , ... ,
   * (n-2, n-1) -> (n-1) + (n-2) + ... + (3) + 1 ,
   * (n-2, n) -> (n-2, n-1) + 1,
   * (n-1, n) -> (n*n - n) / 2
   * 
   * so, f((a,b),n) = sum_(i=1 to a-1){n-i} + (b-a)
   *                = (a-1)*n - (a*a - a)/2 + (b-a)
   */
  int encodeSwap(pair<int,int> swap, int n);

  /**
   * inverse Function of encodeSwap
   * swap and swapCode are bijective
   * 
   * let swapCode is x
   * let decodeSwapCode(x,n) = (a,b)
   * let a0 := (1/2) * {(2n-1) - sqrt((2n-1)^1 -8x)}
   * then a = ceil(a0)
   * and b = x - {n(a-1) - (a^2 + a)/2}
   * 
   * details on github 
   */
  pair<int,int> decodeSwapCode(int swapCode, int n);

  /**
   * ActionHelper::gen~~ returns swaps(vector<pair<int,int> >)
   * 
   * genEpsGreedy :
   *  let eps = tspArgs.EPS
   *  return genGreedy in Prob. 1-eps
   *  return genRandom in Prob. eps
   *  When eps == 1, it is Fully random
   *  When eps == 0, it is Fully Greedy
   * genGreedy :
   *  let method = tspArgs.ACTION_GREEDY_METHOD
   *  if method == "FULL" return genFullGreedy
   *  if method == "PART" return genPartGreedy
   *  if method == "SAMP" return genSampleGreedy
   */ 
  vector<pair<int,int> > genEpsGreedy(State& s, LinearFittedQIteration& LinQ, const Arguments& tspArgs);
  vector<pair<int,int> > genGreedy(State& s, LinearFittedQIteration& LinQ, const Arguments& tspArgs);

  /**
   * This method will be significantly slow
   * but it may be worthy -> O(n^3)...
   * 
   * Strategy:
   *  repeatly search every non-implicated swap of current best swaps
   *  until sigma == n/2 or no more better action found
   * 
   * To do this faster, 
   * Action features are calculated by only differential(差分)
   * of swaps (i.e. new candidate swap )
   */
  vector<pair<int,int> > genFullGreedy(Tour& pi_star, LinearFittedQIteration& LinQ, const Arguments& tspArgs);

  /**
   * Strategy:
   *  repeatly search M numbers of non-implicated swap of current best swaps
   *  until sigma == n/2 or no more better action found in M sampled swaps
   */
  vector<pair<int,int> > genPartGreedy(Tour& pi_star, LinearFittedQIteration& LinQ, const Arguments& tspArgs);

  /**
   * Strategy:
   *  for every sigma in [1,ceil(n/2)],
   *  generate M ramdom swaps.
   *  Compare all of them, and use best scored one
   */
  vector<pair<int,int> > genSampleGreedy(Tour& pi_star, LinearFittedQIteration& LinQ, const Arguments& tspArgs);

  /**
   * return ramdom swaps
   * 
   * let n = tspArgs.V.getN
   * let sigma = random int in [1,n/2]
   * let swapCodeMax = (n*n -n) / 2
   * let swapCodes = randVecBySet(swapCodeMax, sigma)
   * let rst_swaps = {for swapCode in swapCodes : decode(swapCode)}
   * return rst_swaps
   */
  vector<pair<int,int> > genRandom(const Arguments& tspArgs);

  /**
   * Calculate action feature vector by only one swap not entire swaps
   * 
   * let swaps is \sigma dimentional vector of pair<int,int>
   * let pi_vec and pi_inv_vec is Tour's infomation
   * it represent already perterbed tour by swaps from pi_star
   * let's call this tour pi_now
   * let actFeatures is action feature vector of (pi_star, swaps)
   * actFeatures is (_f1, _f2, _f3, _f4, _f5, _f6)
   * 
   * let swaps_new is \sigma+1 dimentional vector of pair<int,int>
   * let swap is next pair of node's index which are going to be swapped
   * then, swaps_new is (swaps, swap) 
   * let pi_next is Tour which is perturbed by swaps_new from pi_star (i.e. perturbed by swap from pi_now)
   * 
   * I want to calculate action feature vector of (pi_star, swaps_new), and it can be calculated by information of actFeature(i.e. action feature vector of (pi_star, swaps)), pi_now and swap.
   * let this feature vector actFeatures_new = (f1,f2,f3,f4,f5,f6)
   * 
   * Calculation of each member of actFeatures_new is like below
   * 1. f1 
   *  f1 = _f1 + 1
   * 2. f2, f3
   *  f2 = _f2 + (added weights by swap)
   *  f3 = _f3 + (dropped weights by swap)
   * 3. f4, f5, f6
   *  let swap is (p,q)
   *  let t = RLmemory.time
   *  let t_p = t - RLmemory.lastTimeActioned[p]
   *  let t_q = t - RLmemory.lastTimeActioned[q]
   *  then, 
   *    f4 = min(_f4, t_p, t_q)
   *    tmpSum = _f5 * (2* _f1) + t_p + t_q
   *    f5 = tmpSum / (2 * f1)
   *    f6 = max(_f6, t_p, t_q)
   * 
   * Finally, return (f1,f2,f3,f4,f5,f6)
   */
  vector<double> getTmpActionFeatures(const vector<double>& actFeatures, vector<int>& pi_vec, vector<int>& pi_inv_vec, pair<int,int>& swap, LinearFittedQIteration& LinQ,const Arguments& tspArgs);

  /**
   * This function do the same thing of ActionHelper::getTmpActionFeatures, but this also updates actFeatures, pi_vec, pi_inv_vec. 
   * 
   * So, it returns nothing
   * and initially operate actFeatures, pi_vec, pi_inv_vec
   */
  void updateActionFeatures(vector<double>& actFeatures, vector<int>& pi_vec, vector<int>& pi_inv_vec, pair<int,int>& swap, LinearFittedQIteration& LinQ, const Arguments& tspArgs);
}

namespace MDPHelper{
  /**
   * Return reward function's value
   * 
   * let f(a,b) = | min (0, (a-b)/b )| = normalImproveReLu(a,b)
   * let d_star is s_next.distPiStar
   * let d_best is LinQ.bestDist
   * let d_l is *(LinQ.bestDist.rbegin() + l - 1) 
   *      (l is from 1 to |LinQ.bestDist| )
   * let alpha is tspArgs.ALPHA
   * 
   * then reward is below
   *  reward = f(d_star, d_best) 
   *           + sum(l = 1 ~ |LinQ.bestDist|){
   *              alpha^(l) * f(d_star, d_l)
   *            }
   */
  double getReward(State& s_next, LinearFittedQIteration& LinQ, const Arguments& tspArgs);
  double normalImproveReLu(double a, double b);

  /**
   * Swap node p and q in pi
   * and return added dist, dropped dist as pair
   * 
   * let swap is (p,q) and p < q in general
   * let i := pi_inv[p], j := pi_inv[q]
   * let d(l,k) means dist(g.getNode(pi[l]), g.getNode(pi[k])) 
   * 
   * First, calculate addedDist
   * it is d(i-1,j) + d(j,i+1) + d(j-1,i) + d(i,j+1)
   * Next, calculate droppedDist
   * it is d(i-1,i) + d(i,i+1) + d(j-1,j) + d(j,j+1)
   * 
   * Then, swap pi[i] and pi[j]
   * and pi_inv[p] := j, pi_inv[q] := i
   * 
   * Finally return pair (addedDist, droppedDist)
   */
  pair<double,double> subPerturb(vector<int>& pi, vector<int>& pi_inv, const pair<int,int>& swap, const Arguments& tspArgs);

  /**
   * Do the same thing of subPerturb,
   * but don't swap pi[p], pi[q]
   * Only calculate added and dropped weights
   */
  pair<double,double> getAddedDroppedWeights(vector<int>& pi, vector<int>& pi_inv, const pair<int,int>& swap, const Arguments& tspArgs);

  /**
   * There are 6 elements in action's feature vector
   * call each of them f_1, ... , f_6
   * and let action's feature vector f_a
   * 
   * information that are needed to calculate f_a is below
   *  1. the numbers of swap in swaps
   *  2. sum of added weights 
   *  3. sum of dropped weights
   *  4. lastTimeNodeActioned
   *  5. time
   * 1. is in Action a
   * 2. and 3. are calculated be subPerturbs
   * 4. and 5. are stored in RLmemory
   * 
   * ## detail about f_4, f_5, f_6 ##
   * let "swaps" is std::vector of std::pair<int,int>
   * let "P" is set of every integral in swaps
   *   ex) swaps = {{2,10}, {6,9}} -> P = {2,10,6,9}
   * let "t" is RLmemory.time
   * let "Tau" is RLmemory.lastTimeNodeActioned
   * let "Tau(P)" is set {Tau(p) | p \in P}
   * 
   * Then f_4 is min(t - Tau(P))
   *   It is equal to t - max(Tau(P))
   * f_5 is average(t - Tau(P))
   *   It is equal to t - average(Tau(P))
   * f_6 is max(t - Tau(P))
   *   It is equal to t - min(Tau(P))
   * 
   * So, while calculating average(Tau(P)),
   * max(Tau(P)), min(Tau(P)) can be also determined
   */
  vector<double> getActionFeatures(Action& a, State& s, LinearFittedQIteration& LinQ, const Arguments& tspArgs);
  vector<double> getActionFeatures(const vector<pair<int,int> >& swaps, Tour& pi_star, LinearFittedQIteration& LinQ, const Arguments& tspArgs);

  /**
   * Return state feature
   * 
   * f_1 ~ f_(3 + KSMP + OMEGA) -> state features
   * f_1 = s.distPiStar
   * f_2 = s.time - s.bestTime
   * f_3 = (s.distPiStar < s.distPiPastStar)
   * f_3+i ( i = 1 ~ KSMP)
   *     = MDPHelper::calcF3i
   * f_3+KSMP+j (j = 1 ~ OMEGA )
   *     = (j-1)th element in MDPHelper::calcF3js
   */
  vector<double> getStateFeatures(State& s, LinearFittedQIteration& LinQ, const Arguments& tspArgs);
  vector<double> calcF3is(State& s, LinearFittedQIteration& LinQ, const Arguments& tspArgs);
  vector<double> calcF3KSMPjs(State& s, LinearFittedQIteration& LinQ, const Arguments& tspArgs);

  /**
   * return f(s,a) = K+1 dimentional double vector
   * 
   * let f_i = f(s,a).at(i) (i = 0 ~ K)
   * let V_SMP = {c_i} (i = 1 ~ KSMP)
   *      c_i is a random city's index which is saved in LinQ
   *      they are all different
   * 
   * f_0 = 1 -> represent constance in weight vector
   * f_1 ~ f_(3 + KSMP + OMEGA) -> state features
   * f_(3 + KSMP + OMEGA) + 1 ~ f_(3 + KSMP + OMEGA) + 6 -> action features
   * 
   * calculate state, action features saperately
   * and merge them later
   */
  vector<double> getFeatureVector(State& s, Action& a, LinearFittedQIteration& LinQ, const Arguments& tspArgs);

  /**
   * Qfunction( action value function) is a mapping 
   * from {Action} x {State} to R (real num).
   * 
   * let st is input State
   * let ac is input Action
   * let K is the demension of feature vector f(st,ac)
   * K is equal to 3 + K_smp + omega + 6
   * let K_st is 3 + K_smp + omega 
   * and K_ac is 6 (i.e. K = K_st + K_ac)
   * let f(st,ac) = ( f_st(st) , f_ac(st, ac) )
   * size of f_st(st) is K_st and f_ac(st, ac) is K_ac
   * 
   * let w is weight vector which is K-dimentional
   * and w_0 is weight constant
   * w is (w_st, w_ac)
   * 
   * <u | v> is dot product
   * 
   * then, Q function is represented like below
   * Q(st, ac) = w_0 + < f(st,ac) | w >
   *           = w_0 + < f_st(st) | w_st > + < f_ac(st,ac) | w_ac >
   *           = w_0 + Q_1(st) + Q_2(st,ac)
   * 
   * Q_1 is evaluateStateFeatures
   * Q_2 is evaluateActionFeatures
   */
  //h = Q(replayBuffer.at(sampleIndex).s_next, a, LinQ.weights);
  double Qfunction(State& s, Action& a, LinearFittedQIteration& LinQ, const Arguments& tspArgs);
  double evaluateActionFeatures(vector<double> actionFeatures, LinearFittedQIteration& LinQ, const Arguments& tspArgs);
}

class State{
  public: //instances
    Tour pi;
    Tour pi_star;
    double distPiStar = 0.0;
    unsigned int time = 1;
    
    //#### for feature mapping
    /**
     * it is  dist(pi_star_{time = this->time-1})
     * default value is DBL_MAX ( = dist(piStar_0))
     * 
     * for f_3
     */
    double distPiPastStar = DBL_MAX; 

    /**
     * t of the smallest dist(piStar_t) (t < this.time)
     * default value is 0 (time of state_0)
     * 
     * when State is constructed by State(s_prev,a_prev,_time,LinQ,args),
     * new State's time will be _time+1.
     * And LinQ.bestTime is that of s_prev, 
     * which is t of the smallest dist(piStar_t) (t < _time ( = this.time-1))
     * So, if s_prev.distPiStat < LinQ.bestDist 
     *        then this.bestTime = _time ( = s_prev.time);
     *     else (LinQ.bestDist is the best solution's dist in t = 1 ~ s_prev.time)
     *        then this.bestTime = LinQ.bestTime
     * 
     * for f_2 
     */
    unsigned int bestTime = 0;

  
  public:
    // constructor 
    State() = default;
    State(Tour pi, Tour pi_star); 
    State(Tour& pi, const Arguments& tspArgs);
    State(State& s_prev, Action& a_prev, LinearFittedQIteration& LinQ, const Arguments& tspArgs);

    // getter 
    Tour getPi();
    Tour getPiStar();

    // perturber
    Tour perturb(Action& a, const Arguments& tspArgs);
};

class Action{
  private:
    int sigma; // the number of swap
    vector<pair <int, int> > swaps; // pairs of swapped indexes

  public:
    Action() = default;
    Action(State& s, LinearFittedQIteration& LinQ, const Arguments& tspArgs);
    vector<pair <int, int> > getSwaps();
    int getSigma();

    //For test
    Action(vector<pair<int,int> > _swaps);
};

class MDP{
  public:
    unsigned int time;
    unsigned int epi;
    unsigned int step;
    State s_prev;
    State s_next;
    Action a_prev;
    double r_prev;
    vector<double> f_prev;
  
  public:
    MDP(State& s_prev, State& s_next, Action& a_prev, double r_prev, vector<double>& f_prev, LinearFittedQIteration& LinQ);
};

#endif // TSP_TMPMDP_H