#ifndef TSP_LINFITQITE_H
#define TSP_LINFITQITE_H

#include <vector>
#include <ctime> // time_t, clock()
#include <deque> 
#include <utility> // std::pair
#include "../model/Arguments.h"
#include "../model/Tour.h"

class DataSet;
class LinearFittedQIteration;

class MDP{
  public:
    int time;
    int epi;
    int step;
    double sec;
    Tour state;
    pair<int,int> action;
    double reward;
    vector<double> featureVector;
  
  public:
    MDP(pair<int,int>& a, double r, LinearFittedQIteration& LinQ);

    void decodeFeatureVector(LinearFittedQIteration& LinQ);
};

class LinearFittedQIteration{
  public:
    vector<double> theta; // weight vector in Qfunction
    deque<MDP> replayBuffer; // save MMAX recent MDPs
    
    // real world's computation time
    time_t startTimeT;
    double spendSec;
    double MAXspendSec; // const SEC_LIMIT in tspArgs

    /**
     * Initial time
     * time, step, epi start from 1
     * time and step are increased when next MDP produced
     * step return to 1 when it become MAXepi+1
     * epi is increased when step return to 1
     * example : MAXstep is 3, MAXepi is 4
     *    time : 1 2 3 4 5 6 7 8 9 10 11 12 end!
     *    step : 1 2 3 1 2 3 1 2 3  1  2  3
     *     epi : 1 1 1 2 2 2 3 3 3  4  4  4 
     */
    int time;
    //  int MAXtime = +infinity
    int step;
    int MAXstep; // const TMAX in tspArgs
    int epi; 
    int MAXepi; // const EPI_LIMIT in tspArgs

    // Focusing Tour : State of now
    Tour s_now;

    //For calculating reward
    Tour bestTour;

    /**
     * Saving data for accelerating
     * 1. Tour length calculation
     *   in searchActionRandom and searchActionGreedy,
     *   amb is always equal to dist(s_next) - dist(s_now)
     *   because of its definition.
     *   Set tourLengthChange to amb.
     *   Then, we don't have to calculate dist(s_next).
     *   dist(s_now) is always known.
     *   dist(s_next) = dist(s_now) + tourLengthChange
     * 
     * 2. Feature vector calculation
     *   let fv(s_now,a_now) is (fs_now,fa_now)
     */
    double tourLengthChange;
    vector<double> featureVector_state;
    vector<pair<int,double> > featureVector_action;

  public:
    // constructor
    LinearFittedQIteration(const Arguments& tspArgs);

    /**
     * Construct Model
     */
    void learn(const Arguments& tspArgs);

    /**
     * check termination condition is satisfied or not
     * return true when algorithm should be terminated
     */
    bool checkTerminationCondition(const Arguments& tspArgs);   

    /**
     * use F2OPT as neighborhood search method
     * because it is the fastest method we have
     * 
     * IJ is an action which make tour shorter
     * 
     * In probability of tspArgs.GREEDY_EPS,
     * return IJ which first found
     *  this is searchActionRandom
     * 
     * In probability of 1 - tspArgs.GREEDY_EPS
     * return IJ which maximize Q-function
     *   this is searchActionGreedy
     * 
     * when there is no improvement in neighborhood,
     * then reset this->s_now by initial solution generator
     * and find IJ again
     */
    pair<int,int> searchAction(const Arguments& tspArgs);

    pair<int,int> searchActionRandom(const Arguments& tspArgs);
    pair<int,int> searchActionGreedy(const Arguments& tspArgs);

    /**
     * reset s_now to the other init Tour
     */
    void resetState(const Arguments& tspArgs);

    /**
     * reset featureVector_state of this->s_now
     */
    void setFeatureVector_state(const Arguments& tspArgs);

    /**
     * reset featureVector_state of action
     * 
     * let featureVector_state as FA in short
     * 
     * FA is vector of pair<int,double>
     * first member represents index
     * second member represents value
     * 
     * FA is very sparce,
     * so this representation makes sense
     */
    void setFeatureVector_action(pair<int,int> action, const Arguments& tspArgs);

    /**
     * return reward
     * 
     * r = alpha**((bestTour.getCost / (s_now.getCost + tourLengthCange)) -1 ) + beta*((s_now.getCost / (s_now.getCost + tourLengthCange)) -1 )
     */
    double calcReward(const Arguments& tspArgs);

    /**
     * return action's value
     */
    double calcActionValue(int p,int pp, int q, int qp, const Arguments& tspArgs);

    /**
     * finish a time step, and update model's internal infomation
     * 
     * 1. update ReplayBuffer
     * 2. update s_now
     * 3. update BestTour (BestTour vs s_now)
     * 4. update featureVector_state
     * 5. update time, step, spendSec
     * 6. if step > MAXstep then
     * 7.   step = 1
     * 8.   update epi
     * 9.   update theta
     */
    void update(pair<int,int>& a_now, MDP& mdp_now, const Arguments& tspArgs);

    /**
     * Push_back prevMDP into ReplayBuffer
     * and pop_front if its size is bigger than MMAX
     */
    void updateReplayBuffer(MDP& mdp_now, const Arguments& tspArgs);

    /**
     * Conduct 2-opt-move to s_now
     * 
     * update Tour and its cost
     */
    void updateStateNow(pair<int,int>& a_now);

    /**
     * Update featureVector_state with featureVector_action
     */
    void updateFeatureVectorState();

    /**
     * Update theta with LP solver CPLEX
     */
    void updateTheta(const Arguments& tspArgs);

    // Getter
    //vector<double> getTheta();

    //For Debugging
    //void printTheta();
};


class DataSet{
  public:
    vector<int> sampleIndexs; // indexs of replayBuffer
    //"indices" is right english, but "indexs" is easier to understand

    /**
     * fitted Q iteration find more fitted Q function(i.e. model).
     * In this program, Q function is inner product of weight vector and feature vector.
     * and, better model has less least square error of target value.
     * 
     * let y is T-dimentional vector of target values
     * let X is (K+1)*T matrix of feature vectors and 1 (vectical)
     * let w is (K+1) dimention vector (called weight vector)
     * 
     * then, model try to find w, w0 which minimize below value
     *  LSM := l1norm(y - (Xw))
     */
    vector<double> targetValues; // target values for model regression
    vector< vector<double> > featureVectors; // model's input data

  public:
    DataSet() = default;
    DataSet(const Arguments& tspArgs, LinearFittedQIteration& LinQ);

  // for Debugging
  public:
    void printInfo();
};

#endif //TSP_LINFITQITE_H