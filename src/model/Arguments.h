#ifndef TSP_ARGUMENTS_H
#define TSP_ARGUMENTS_H

#include <string>
#include <vector>
#include "../model/Graph.h"

/**
 * 強化学習に必要なすべての伝達因子(argument)を１つのオブジェクトとして扱う
 * このオブジェクトは一度生成された後、修正されない
 * しかし、デバッグを楽にするため、constには生成しない
 * 使うときに！！必ず！！const Arguments& tspArgs のように使用
 * 各メンバー変数をpublicに宣言し、. operatorでアクセスできるようにする。
 */

class Arguments {
  public : 
    //==========================================
    //===== Input arguments ===================
    //==========================================
    string TSP_DATA_FILE ; // file name of tsp instance
    string INIT_SOLUTION_METHOD ; // name of init solution generator
    string LOCAL_SEARCH_METHOD ; // name of local-opt searcher
    string TERMINATE_METHOD; // name of termination method (time or episode)
    string ACTION_GREEDY_METHOD; // name of greedy algorithm in Action constructor
    string WEIGHT_INIT_METHOD; // name of weight initialization method

    unsigned int SEED ; // seed of RNG
    unsigned int T ; // length of Replay Buffer
    unsigned int TMAX ; // length of Samples
    unsigned int THETA ; // parameter in reward function
    unsigned int EPI_LIMIT ; // Maximum value of episode number
    unsigned int MMAX ; // Maximum size of MDP queue

    double GAMMA ; // parameter for calculating target values. in [0,1]
    double ALPHA ; // parameter in reward function. in [0,1]
    double SEC_LIMIT ; // Maximum value of second
    double WEIGHTS_INITPARA ; // Interval of initial weight vector or Standard deviation of gaussian
    double EPS ; // parameter of epsilon greedy in Action constructor in [0,1]
    double EPS_SF2OPT; // parametor in scaled fast 2-opt operator. skip swap when variation of distance is smaller than EPS_SF2OPT

    //==========================================
    //===== Arguments that are calculated ======
    //==========================================
    unsigned int K ; // lenght of feature vector and weight vector(minus 1)
    unsigned int KSMP ; // the number of sample node in feature mapping (state part)
    unsigned int OMEGA ; // the number of largest sum of edge weight in feature mapping (state part)
    unsigned int ASMP ; // the number of samples about every sigma in Action's greedy algorithm
    unsigned int SIGMAMAX ; // maximum length of action
    unsigned int F3IMAX ; // maximum number of edges to check on calculating state feature's f_(3+i)

    Graph V; // set of nodes
  public :
    Arguments();

    Arguments(vector<string>& stringArgs, vector<unsigned int>& integerArgs, vector<double>& realArgs);
};

#endif // TSP_ARGUMENTS_H