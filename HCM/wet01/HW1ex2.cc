#include <errno.h>
#include <signal.h>
#include <sstream>
#include <fstream>
#include <queue>
#include <set>
#include <map>
#include <algorithm>
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

  // flatten the design to operate on a flat netlist
  hcmCell *flatCell = hcmFlatten(cellName + string("_flat_rank"), topCell, globalNodes);

  // gather instances
  map<string, hcmInstance *> allInsts = flatCell->getInstances();

  // ranking via topological propagation
  const string TOP_SRC = "__TOP_INPUT__";
  const int NEG_INF = -1000000;
  map<string, int> rank;
  map<string, int> indeg;
  map<string, vector<string>> adj;

  // init rank/indeg for all instances
  for (auto &ipair : allInsts)
  {
    rank[ipair.first] = NEG_INF;
    indeg[ipair.first] = 0;
  }
  rank[TOP_SRC] = -1;
  indeg[TOP_SRC] = 0;

  // build edges driver->sink
  set<pair<string, string>> edges;
  for (auto &npair : flatCell->getNodes())
  {
    hcmNode *node = npair.second;
    if (globalNodes.find(node->getName()) != globalNodes.end())
    {
      continue;
    }

    vector<string> drivers;
    vector<string> sinks;

    for (auto &ippair : node->getInstPorts())
    {
      hcmInstPort *ip = ippair.second;
      hcmPort *mport = ip->getPort();
      if (!mport)
      {
        continue;
      }
      string iname = ip->getInst()->getName();
      if (mport->getDirection() == OUT || mport->getDirection() == IN_OUT)
      {
        drivers.push_back(iname);
      }
      if (mport->getDirection() == IN || mport->getDirection() == IN_OUT)
      {
        sinks.push_back(iname);
      }
    }

    hcmPort *p = node->getPort();
    if (p && (p->getDirection() == IN || p->getDirection() == IN_OUT))
    {
      drivers.push_back(TOP_SRC);
    }

    for (const auto &d : drivers)
    {
      for (const auto &s : sinks)
      {
        if (d == s)
        {
          continue;
        }
        pair<string, string> e(d, s);
        if (edges.insert(e).second)
        {
          adj[d].push_back(s);
          indeg[s]++; // ensure entry exists
          if (indeg.find(d) == indeg.end())
          {
            indeg[d] = 0;
          }
        }
      }
    }
  }

  // Kahn topological traversal, longest-path style
  queue<string> q;
  for (auto &ipair : indeg)
  {
    if (ipair.second == 0)
    {
      if (rank.find(ipair.first) == rank.end())
      {
        rank[ipair.first] = NEG_INF;
      }
      q.push(ipair.first);
    }
  }

  while (!q.empty())
  {
    string cur = q.front();
    q.pop();
    int curRank = rank[cur];
    for (const auto &nxt : adj[cur])
    {
      if (curRank != NEG_INF && rank[nxt] < curRank + 1)
      {
        rank[nxt] = curRank + 1;
      }
      indeg[nxt]--;
      if (indeg[nxt] == 0)
      {
        q.push(nxt);
      }
    }
  }

  // build and sort output
  for (auto &ipair : rank)
  {
    if (ipair.second >= 0 && ipair.first != TOP_SRC)
    {
      maxRankVector.push_back(make_pair(ipair.second, ipair.first));
    }
  }
  sort(maxRankVector.begin(), maxRankVector.end(),
       [](const pair<int, string> &a, const pair<int, string> &b)
       {
         if (a.first != b.first)
           return a.first < b.first;
         return a.second < b.second;
       });

  for(auto itr = maxRankVector.begin(); itr != maxRankVector.end(); itr++){
		fv << itr->first << " " << itr->second << endl;
	}

  return(0);
}
