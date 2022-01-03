#ifndef TSP_LINFITQITE_H
#define TSP_LINFITQITE_H

#include <vector>
#include <ctime> // time_t, clock()
#include <deque> 
#include <utility> // std::pair
#include "../model/Arguments.h"
#include "../model/Tour.h"

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
    unsigned int time;
    //  unsigned int MAXtime = +infinity
    unsigned int step;
    unsigned int MAXstep; // const TMAX in tspArgs
    unsigned int epi; 
    unsigned int MAXepi; // const EPI_LIMIT in tspArgs

    // Focusing Tour : State of now
    Tour s_now;

    //For calculating reward
    Tour bestTour;

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
     * 
     */
    pair<int,int> searchAction(const Arguments& tspArgs);

    /**
     * finish a time step, and update model's internal infomation
     * 
     * 1. update replayBuffer
     * 2. update bestInfos
     * 3. s_prev = s_next
     * 4. this->time ++
     */
    //void update(???);


    /**
     * Push_back prevMDP into ReplayBuffer
     * and pop_front if its size is bigger than MMAX
     */
    //void updateReplayBuffer(MDP& prevMDP,const Arguments& tspArgs);

    /**
     * Update bestTime and bestDist
     * 
     * let input double number dist_prev
     * 
     * if bestDist > dist_prev
     * then bestDist = dist_prev 
     * and bestTime = time
     */
    //void updateBestInfos(State& s_prev);

    /**
     * update weights vector using least square method
     */
    //void updateTheta(DataSet& dataSet);

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

class MDP{
  public:
    unsigned int time;
    unsigned int epi;
    unsigned int step;
    Tour state;
    pair<int,int> action;
    double reward;
    vector<double> featureVector;
  
  public:
    MDP(Tour& s, pair<int,int>& a, double r, vector<double>& f, LinearFittedQIteration& LinQ);
};


#endif //TSP_LINFITQITE_H