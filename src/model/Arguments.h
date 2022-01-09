#ifndef TSP_ARGUMENTS_H
#define TSP_ARGUMENTS_H

#include <string>
#include <vector>
#include <map>
#include <random>
#include "../model/Graph.h"

/**
 * 強化学習に必要なすべての伝達因子(argument)を１つのオブジェクトとして扱う
 * このオブジェクトは一度生成された後、修正されない
 * しかし、デバッグを楽にするため、constには生成しない
 * 使うときに！！必ず！！const Arguments& tspArgs のように使用
 * 各メンバー変数をpublicに宣言し、. operatorでアクセスできるようにする。
 * getter を作った方が安全だが、const で渡せば安全に. だけで同じことができる
 */

class Arguments {
  public : 
    //==========================================
    //===== Input arguments ===================
    //==========================================
    string TSP_INSTANCE_NAME ; // file name of tsp instance
    string TOUR_INIT_METHOD ; // name of init solution generator
    string LEARN_TERMINATE_METHOD; // name of termination method (time or episode)
    string THETA_INIT_METHOD; // name of weight initialization method

    int SEED ; // seed of RNG
    int T ; // the number of samples for each learning
    int TMAX ; // the number of steps per theta update
    int MMAX ; // Maximum size of MDP queue
    int LAMBDA ; // parameter in calculating rho
    int HMIN ; // minimum axis divisions in partition
    int HMAX ; // maximum axis divisions in partition
    int EPI_LIMIT ; // Maximum value of episode number

    double ALPHA ; // parameter in reward function. bigger than 1
    double BETA ; // parameter in reward function. in [0,1]
    double GAMMA ; // discount factor. in [0,1]
    double THETA_INITPARA ; // Interval of initial weight vector or Standard deviation of gaussian
    double GREEDY_EPS ; // parameter of epsilon greedy in Action constructor in [0,1]
    double SEC_LIMIT ; // Maximum value of second

    mt19937 rng_tourInit;

    //==========================================
    //===== Arguments that are calculated ======
    //==========================================
    Graph V; // set of nodes

    map<int, vector<int> > partitions;
    int K0; // sum(h= HMIN ~ HMAX) (h**4 + h**2)/2
    int K ; // lenght of feature vector and learning parameter vector (minus 1)
    int StateBegin;
    int ActionBegin;
    map<int,int> partitionBegins;
    double U;

  public :
    Arguments();

    Arguments(vector<string>& stringArgs, vector<int>& integerArgs, vector<double>& realArgs);

    void setPartitions();
    void setPartitionBegins();
    void setU();
    void resetRNG();
};

#endif // TSP_ARGUMENTS_H