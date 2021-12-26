#ifndef TSP_FILEIOHELPER_H
#define TSP_FILEIOHELPER_H

#include <vector>
#include <string>

using namespace std;

/**
 * Define below directories.
 */
#define DATA_DIR "../data/TSPLIB/" // in ./build
//#define DATA_DIR "../../data/TSPLIB/" // in ./debug ./model

/**
 * Convert TSP data file name to its absolute file path.
 * 
 * @param file_name
 * @return its absolute file path
 */
string convertToDataPath(const string& file_name);

/**
 * Read the node data set from TSP data file.
 * 
 * @param file_name
 * @return vector of node datas
 */
vector<vector <double> > readTSPFile(const string& file_name);

#endif //TSP_FILEIOHELPER_H