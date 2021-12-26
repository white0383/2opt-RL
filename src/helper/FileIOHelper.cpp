#include "FileIOHelper.h"
#include <fstream>
#include <sstream>
#include <iostream>

string convertToDataPath(const string& file_name){
  string dataPath(DATA_DIR);
  dataPath.append(file_name);
  dataPath.append(".tsp");

  return dataPath;
};

vector<vector <double> > readTSPFile(const string& file_name){
  // Open the data file
  ifstream in(convertToDataPath(file_name));

  // Check the file successfully opened
  if (in.fail()) {
    cout << "ERROR: Could not open the file!" << endl;
    cout << "yout fileName : " << file_name << endl;
    //exit(1);
  }

  vector< vector< double > > node_datas;
  string line_buffer;
  double index, x, y;

  // Skip while "NODE_COORD_SECTION" appears
  while(getline(in, line_buffer)){
    if(line_buffer == "NODE_COORD_SECTION") break;
    if(line_buffer == "EOF"){
      cout << file_name <<" is not euc-2d TSP" << endl;
      exit(1);
    }
  }

  // Read line by line
  while (getline(in, line_buffer)){
    istringstream iss(line_buffer);
    
    // if stream meet "EOF" then break
    if (line_buffer == "EOF") break;
    
    iss >> index >> x >> y;
    node_datas.push_back({index, x, y}); // Add to node data set
  }

  in.close(); // close data file

  return node_datas;
};