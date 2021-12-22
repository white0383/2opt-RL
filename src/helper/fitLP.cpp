#include "fitLP.h"
#include <ilcplex/ilocplex.h>

using namespace std;

// true when debugging
#define DEBUG_FITLP false

vector<double> fitLP(const vector< vector< double > >& X, const vector<double>& y){
  int T = y.size();
  int K = X[0].size();
  int i,j; // for loop index

  IloEnv env;

  IloModel model(env);
  IloNumVarArray vars(env);
  IloRangeArray con(env);

  // declare variables
  for(i=0; i<T+K+1; i++){ 
    vars.add(IloNumVar(env)); // w0 ~ wK , z1 ~ zT
  }

  // set objective function
  IloExpr obj(env);
  for(i=0; i<T; i++){
    obj += vars[K+1+i]; // obj = sum(z1 ~ zT)
  }

  // objective function is minimizer
  IloObjective objective(env);
  objective.setExpr(IloMinimize(env, obj));
  model.add(objective);
  
  //declare constraints
  IloExpr cons(env);
  IloExpr hi(env);

  for(i=0; i<T;i++){
    hi = vars[0]; // w0
    for(j=0; j<K; j++){
      hi += vars[j+1] * X[i][j]; // X_{i,j} * w_j
    }
    cons = y[i] - hi;

    con.add( cons - vars[K+1+i] <= 0); // cons_i - z_i <= 0
    con.add(-cons - vars[K+1+i] <= 0); // -cons_i - z_i <= 0
  }

  cons.end();
  hi.end();

  model.add(con);

  // solve LP
  IloCplex cplex(model);
  if(!(DEBUG_FITLP)) cplex.setOut(env.getNullStream());
  cplex.solve();

  // save new weight
  IloNumArray vals(env);

  double double_vals;
  for(i=0; i<T+K+1; i++){
    try{
      double_vals = cplex.getValue(vars[i]);
    } catch(...){
      double_vals = 0;
    }
    vals.add(double_vals);
  }

  if(DEBUG_FITLP) env.out() << "Solution Value = " << cplex.getObjValue() << endl;
  if(DEBUG_FITLP) env.out() << "Values = " << vals << endl;

  // new weight
  vector<double> rst_w(K+1);
  for(i=0; i<K+1; i++){
    rst_w[i] = vals[i];
  }

  env.end( );

  return rst_w;
}