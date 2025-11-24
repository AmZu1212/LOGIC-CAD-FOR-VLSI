#include <fstream>
#include <map>
#include <set>
#include <vector>

using namespace std;

/**
 * hcmSigVec class will mange the parsing of the signals and vector files.
 * once instantiated will open up the given signals and vectors files provided in the constructor,
 * prepare the list of signals and be ready for further calls to read each line of the vectors file
 * and answer queries about specific signals value.
 * hcmSigVec is a mutable object.
 */
class hcmSigVec {
  private:
    // boolean variable to determine whether to print comments
    bool verbose;
    // true if the parsers are OK, false otherwise
    bool isGood;
    // holds the current line in the vec.txt file
    unsigned int vecLineNum;
    // Input stream class to operate on sig.txt file
    ifstream sigs;
    // Input stream class to operate on vec.txt file
    ifstream vecs;
    // name of the sig.txt file
    string sigsFileName;
    // name of the vec.txt file
    string vecsFileName;
    // signals value by the signal idx
    vector<bool> sigValsByIdx; 
    // mapping from signal name to idx
    map<string, int> signalVecIdx; 

    /** @fn int parseSignalsFile()
     * @brief create a mapping between signal to it's index
     * @return 0 on success
     */
    int parseSignalsFile();

  public:
    /** @fn hcmSigVec(string sigsFileName, string vecsFileName, bool verbose = false)
     * @brief hcmSigVec constractor.
     * @param sigsFileName - name of the signal file 
     * @param vecsFileName - name of the vector file 
     * @param verbose - boolean variable to determine whether to print comments
     */
    hcmSigVec(string sigsFileName_, string vecsFileName_, bool verbose_ = false);

    /** @fn bool good()
     * @brief gets the status of the parsing if successful or not.
     * @return true if the parsers are OK, false otherwise
     */
    bool good() {return isGood; };

    /** @fn int readVector()
     * @brief read a line of the vector file and update the releavnet container
     * @return -1 if could not read since EOF\n
     * 1 if an error occurred\n
     * 0 if the operation succeeded 
     */
    int readVector();

    /** @fn int getSigValue(string sigName, bool& val)
     * @brief get the value of the given signal name.\nSupport only single bit values.
     * @param sigName - name of the signal
     * @param val - refernce to boolean return value
     * @return 1 if the signal unknown\n
     * 0 if the operation succeeded  
     */
    int getSigValue(string sigName, bool& val);

    /** @fn int getSignals(set<string>& signals)
     * @brief gets the number of signals and fill the given set with names of the signals.
     * @param signals - refernce to set<string> to contain the names of the signals.
     * @return number of signals.
     */
    int getSignals(set<string>& signals);
};




