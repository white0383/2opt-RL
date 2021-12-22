#ifndef TSP_LINFITQITE_H
#define TSP_LINFITQITE_H

#include <vector>
#include <ctime> // time_t, clock()
#include <deque> 
#include <utility> // std::pair
#include "../model/Arguments.h"
#include "../model/Tour.h"
#include "MarkovDecisionProcess.h"

class LinearFittedQIteration{
  public:
    vector<double> weights;
    deque<MDP> replayBuffer;
    
    // real world's computation time
    time_t startTimeT;
    double spendSec;
    double MAXspendSec; // const

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
    unsigned int time;
    //unsigned int MAXtime = +infinity
    unsigned int step;
    unsigned int MAXstep; // const
    unsigned int epi; 
    unsigned int MAXepi; // const

    //For calculating feature vector and reward
    unsigned int bestTime;
    double bestDist;
    vector<unsigned int> lastTimeNodeActioned;
    deque<double> distQueue; // maximum length is THETA in tspArgs
    vector<int> V_smp;

    //For showing performance
    Tour bestTour;

  public:
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
     * finish a time step, and update model's internal infomation
     * 
     * 1. update replayBuffer
     * 2. update Tau (LastTimeNodeActioned)
     * 3. update bestInfos
     * 4. update distQ
     * 5. s_prev = s_next
     * 6. this->time ++
     */
    void updateInfo(MDP& mdp, State& s_prev, State& s_next, const Arguments& tspArgs);


    /**
     * Push_back prevMDP into ReplayBuffer
     * and pop_front if its size is bigger than MMAX
     */
    void updateReplayBuffer(MDP& prevMDP,const Arguments& tspArgs);

    /**
     * Update lastTimeNodeActioned by current action
     * 
     * let current action is {sigma, {pair_i (i = 1 ~ sigma)}}
     * let pair_i is (p_i, q_i) 
     * let lastTimeNodeAcitoned is called as Tau
     * 
     * then, for every i = 1 ~ sigma,
     *    Tau[p_i], Tau[q_i] = time
     */
    void updateLastTimeNodeActioned(Action& a_prev);

    /**
     * Update bestTime and bestDist
     * 
     * let input double number dist_prev
     * 
     * if bestDist > dist_prev
     * then bestDist = dist_prev 
     * and bestTime = time
     */
    void updateBestInfos(State& s_prev);

    /**
     * let input double number as dist_next
     * 
     * push_back dist_next into distQueue
     * and pop_front if its size is bigger thand THETA
     */
    void updateDistQueue(double dist_piStar_next, const Arguments& tspArgs);
    
    /**
     * update weights vector using least square method
     */
    void updateWeights(DataSet& dataSet);

    // Getter
    vector<double> getWeights();

    //For Debugging
    void printWeights();
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
     *  LSM := l2norm(y - (Xw))
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