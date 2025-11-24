#include <errno.h>
#include <signal.h>
#include <sstream>
#include <fstream>
#include "hcm.h"
#include "flat.h"

using namespace std;

bool verbose = false;


int main(int argc, char **argv) {
  int argIdx = 1;
  int anyError = 0;
  unsigned int i;
  vector<string> vlgFiles;
  
  if (argc < 3) {
    anyError++;
  } else {
    if (!strcmp(argv[argIdx], "-v")) {
      argIdx++;
      verbose = true;
    }
    for (;argIdx < argc; argIdx++) {
      vlgFiles.push_back(argv[argIdx]);
    }
    
    if (vlgFiles.size() < 2) {
      cerr << "-E- At least top-level and single verilog file required for spec model" << endl;
      anyError++;
    }
  }

  if (anyError) {
    cerr << "Usage: " << argv[0] << "  [-v] top-cell file1.v [file2.v] ... \n";
    exit(1);
  }
 
  set< string> globalNodes;
  globalNodes.insert("VDD");
  globalNodes.insert("VSS");
  
  hcmDesign* design = new hcmDesign("design");
  string cellName = vlgFiles[0];
  for (i = 1; i < vlgFiles.size(); i++) {
    printf("-I- Parsing verilog %s ...\n", vlgFiles[i].c_str());
    if (!design->parseStructuralVerilog(vlgFiles[i].c_str())) {
      cerr << "-E- Could not parse: " << vlgFiles[i] << " aborting." << endl;
      exit(1);
    }
  }
  

  /*direct output to file*/
  string fileName = cellName + string(".rank");
  ofstream fv(fileName.c_str());
  if (!fv.good()) {
    cerr << "-E- Could not open file:" << fileName << endl;
    exit(1);
  }

  hcmCell *topCell = design->getCell(cellName);
  if (!topCell) {
    printf("-E- could not find cell %s\n", cellName.c_str());
    exit(1);
  }
  
  fv << "file name: " << fileName << endl;
  

  /* assign your answer for HW1ex2 to maxRankVector 
   * maxRankVector is a vector of pairs of type <int, string>,
   * int is the rank of the instance,
   * string is the name of the instance
   * Note - maxRankVector should be sorted.
  */
  vector<pair<int, string>> maxRankVector;
  //---------------------------------------------------------------------------------//
  //enter your code here 
  //---------------------------------------------------------------------------------//
  for(auto itr = maxRankVector.begin(); itr != maxRankVector.end(); itr++){
		fv << itr->first << " " << itr->second << endl;
	}

  return(0);
}
