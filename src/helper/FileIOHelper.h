#ifndef TSP_FILEIOHELPER_H
#define TSP_FILEIOHELPER_H

#include <vector>
#include <string>
#include "../model/Node.h"

using namespace std;

/**
 * Define below directories.
 */
#define DATA_DIR "../data/TSP_VLSI_DATASET/"
#define RESULT_DIR "../result/TSP_VLSI_DATASET/"
#define USAGE_DIR "../data/USAGE/usage.txt"

/**
 * Convert TSP data file name to its absolute file path.
 * 
 * @param file_name
 * @return its absolute file path
 */
string convertToDataPath(const string& file_name);

/**
 * Convert TSP data file name to absolute file path of result tour data file
 * 
 * @param file_name
 * @return absolute file path of result tour data file
 */
string convertToResultPath(const string& file_name);

/**
 * Read the node data set from TSP data file.
 * 
 * @param file_name
 * @return vector of node datas
 */
vector<vector <int> > readTSPFile(const string& file_name);

/**
 * Write txt file of tour infomation
 * 
 * @param file_name, tour data
 */
// void writeTourAsFile(const string file_name, Tour &tour)

/**
 * Print txt file on command line
 * 
 * @param file_path
 */
void printFileOnConsole(const string& file_path);

/**
 * Print usage file on command line
 */
void printUsage();

#endif //TSP_FILEIOHELPER_H