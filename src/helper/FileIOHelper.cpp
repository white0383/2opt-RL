#include "FileIOHelper.h"
#include <fstream>
#include <sstream>
#include <iostream>

string convertToDataPath(const string& file_name){
  string dataPath(DATA_DIR);
  dataPath.append(file_name);

  return dataPath;
};

string convertToResultPath(const string& file_name){
  string resultPath(RESULT_DIR);
  resultPath.append(file_name);

  return resultPath;
};

vector<vector <int> > readTSPFile(const string& file_name){
  // Open the data file
  ifstream in(convertToDataPath(file_name));

  // Check the file successfully opened
  if (in.fail()) {
    cout << "ERROR: Could not open the file!" << endl;
    cout << "yout fileName : " << file_name << endl;
    exit(1);
  }

  vector< vector< int > > node_datas;
  string line_buffer;
  int index, x, y;

  // Skip first 8 lines (there are only comments)
  for (int i = 0; i<8;i++){
    getline(in, line_buffer);
  }

  // Read line by line
  while (getline(in, line_buffer)){
    istringstream iss(line_buffer);

    //I don't know why, but even though when iss.str() is "EOF",
    //iss.str().compare("EOF") is always 1
    if (iss.str().compare("EOF") == 1) break ;
    
    iss >> index >> x >> y;

    node_datas.push_back({index, x, y}); // Add to node data set
  }

  in.close(); // close data file

  return node_datas;
};

// void writeTourAsFile(const string file_name, Tour &tour) {};

void printFileOnConsole(const string& file_path){
  ifstream in(file_path);
  string content;

  if (in.fail()) {
    cout << "ERROR: Could not open the file!" << endl;
    exit(1);
  } else {
    in.seekg(0, ios::end);
    int size = in.tellg();
    content.resize(size);
    in.seekg(0, ios::beg);
    in.read(&content[0], size);
  }

  in.close();
}

void printUsage(){
  printFileOnConsole(USAGE_DIR);
}

