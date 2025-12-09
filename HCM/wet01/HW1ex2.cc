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
#define NEG_INF -1000000 // important to signify the default rank.
#define TOP_SRC "__TOP_INPUT__"
using namespace std;

// Helper Functions Declarations

/**
 * @brief Build driver -> sink edges for a flattened netlist.
 *
 * Skips global nodes, classifies instPorts by direction (IN->sink, OUT->driver,
 * IN_OUT->both), and adds a synthetic TOP_SRC for top inputs. Each unique
 * driver/sink pair updates @p edges, fills @p adj, and bumps sink indegree;
 * drivers are seeded with indegree 0.
 */
void BuildEdges(hcmCell *flatCell, set<string> &globalNodes, map<string, vector<string>> &adj, map<string, int> &indeg, set<pair<string, string>> &edges);
/// Collect all non-top entries with non-negative rank into the result vector.
void InsertResult(vector<pair<int, string>> &maxRankVector, map<string, int> &rank);
/// Sort results by rank, then name (lexicographic) in-place.
void SortResults(vector<pair<int, string>> &maxRankVector);

/// Emit each (rank, name) pair to the output stream, one per line.
void PrintResults(const vector<pair<int, string>> &maxRankVector, ofstream &fv);

bool verbose = false;

int main(int argc, char **argv)
{
	int argIdx = 1;
	int anyError = 0;
	unsigned int i;
	vector<string> vlgFiles;

	if (argc < 3)
	{
		anyError++;
	}
	else
	{
		if (!strcmp(argv[argIdx], "-v"))
		{
			argIdx++;
			verbose = true;
		}
		for (; argIdx < argc; argIdx++)
		{
			vlgFiles.push_back(argv[argIdx]);
		}

		if (vlgFiles.size() < 2)
		{
			cerr << "-E- At least top-level and single verilog file required for spec model" << endl;
			anyError++;
		}
	}

	if (anyError)
	{
		cerr << "Usage: " << argv[0] << "  [-v] top-cell file1.v [file2.v] ... \n";
		exit(1);
	}

	set<string> globalNodes;
	globalNodes.insert("VDD");
	globalNodes.insert("VSS");

	hcmDesign *design = new hcmDesign("design");
	string cellName = vlgFiles[0];
	for (i = 1; i < vlgFiles.size(); i++)
	{
		printf("-I- Parsing verilog %s ...\n", vlgFiles[i].c_str());
		if (!design->parseStructuralVerilog(vlgFiles[i].c_str()))
		{
			cerr << "-E- Could not parse: " << vlgFiles[i] << " aborting." << endl;
			exit(1);
		}
	}

	/*direct output to file*/
	string fileName = cellName + string(".rank");
	ofstream fv(fileName.c_str());
	if (!fv.good())
	{
		cerr << "-E- Could not open file:" << fileName << endl;
		exit(1);
	}

	hcmCell *topCell = design->getCell(cellName);
	if (!topCell)
	{
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

	// Written by:
	//				Amir Zuabi 		 - 212606222
	//				Alexey Vasilayev - 323686683
	//  === HW1ex2 ANSWER STARTS HERE ===

	// we went for a BFS style approach, where we start with "source" nodes, and we "tick off" nodes as we process their drivers, to then be the next sources etc.

	// we have some long lines ahead, we recommend using "code wrapping" in your editor of choice.

	// *** please note, the helper functions have comments explaining their purpose in the declarations in the top of this file. the definitions are at the bottom of this file.

	// get a flat the design
	hcmCell *flatCell = hcmFlatten(cellName + string("_flat_rank"), topCell, globalNodes);

	// get all instances
	map<string, hcmInstance *> allInsts = flatCell->getInstances();

	// ranking via source propagation (topological order)
	map<string, vector<string>> adj;
	map<string, int> rank;
	map<string, int> indeg;

	// initialize all "vertices" ranks to -inf
	for (auto &instance : allInsts)
	{
		rank[instance.first] = NEG_INF;
		indeg[instance.first] = 0;
	}

	// TOP_SRC is a fake outer driver. so its on level "-1"
	rank[TOP_SRC] = -1;
	indeg[TOP_SRC] = 0;

	// build edges driver->sink
	set<pair<string, string>> edges;
	BuildEdges(flatCell, globalNodes, adj, indeg, edges);

	// init queue with only the initial source nodes
	queue<string> sourceQueue;
	for (auto &instance : indeg)
	{
		// only insert nodes without "pre-requisites" (i.e a source node)
		if (instance.second == 0)
		{
			if (rank.find(instance.first) == rank.end())
			{
				rank[instance.first] = NEG_INF;
			}
			sourceQueue.push(instance.first);
		}
	}

	// run BFS style rank propogation, when a node is "ready", i.e all its drivers where processed, we add it to the source queue.
	while (!sourceQueue.empty())
	{
		// get head
		string cur = sourceQueue.front();
		sourceQueue.pop();
		int curRank = rank[cur];

		for (const auto &next : adj[cur])
		{
			// if the next rank is smaller than current + 1, update it
			if (curRank != NEG_INF && rank[next] < curRank + 1)
			{
				rank[next] = curRank + 1;
			}

			// mark port as done (-1 source), and if 0, insert to "drivers" queue
			indeg[next]--;
			if (indeg[next] == 0)
			{
				sourceQueue.push(next);
			}
		}
	}

	// insert, sort & print results
	InsertResult(maxRankVector, rank);
	// please note:
	//		since we sort the results (O(VlogV)), it is technically not O(V),
	//		but since our algorithm is O(V+E), and in this case E is some multiple of
	//		V, we will say it is linear in the number of instances.
	SortResults(maxRankVector);
	PrintResults(maxRankVector, fv);

	return (0);
}

void BuildEdges(hcmCell *flatCell, set<string> &globalNodes, map<string, vector<string>> &adj, map<string, int> &indeg, set<pair<string, string>> &edges)
{
	for (auto &nodeItr : flatCell->getNodes())
	{
		// get current node
		hcmNode *node = nodeItr.second;

		// skip global nodes
		if (globalNodes.find(node->getName()) != globalNodes.end())
			continue;

		// we want to split ports into drivers and sinks
		vector<string> drivers;
		vector<string> sinks;

		// insert fake top driver if needed
		hcmPort *port = node->getPort();
		if (port != nullptr)
		{
			hcmPortDir dir = port->getDirection();
			if (dir == IN || dir == IN_OUT)
			{
				drivers.push_back(TOP_SRC);
			}
		}

		// classify whether it is in/out/both
		for (auto &portItr : node->getInstPorts())
		{
			hcmInstPort *port = portItr.second;
			hcmPort *masterPort = port->getPort();
			if (!masterPort)
				continue; // null guard

			// insert based on enum type
			string instanceName = port->getInst()->getName();
			switch (masterPort->getDirection())
			{
			case IN:
				sinks.push_back(instanceName);
				break;
			case OUT:
				drivers.push_back(instanceName);
				break;
			case IN_OUT:
				sinks.push_back(instanceName);
				drivers.push_back(instanceName);
				break;
			default:
				continue;
			}
		}

		// build edges
		for (const auto &driver : drivers)
		{
			// add driver if doesnt exists yet
			if (indeg.find(driver) == indeg.end())
			{
				indeg[driver] = 0;
			}

			for (const auto &sink : sinks)
			{
				// ignore in-outs (it is not a real edge)
				if (driver == sink)
					continue;

				// create edge
				pair<string, string> edge(driver, sink);

				// insert returns true to the second element if successful
				auto result = edges.insert(edge);

				// update adj & indeg
				if (result.second)
				{
					// connect driver to sink
					adj[driver].push_back(sink);
					indeg[sink]++;
				}
			}
		}
	}
}

void SortResults(vector<pair<int, string>> &maxRankVector)
{
	// sort by rank, then by name
	sort(maxRankVector.begin(), maxRankVector.end(),
		 [](const pair<int, string> &a, const pair<int, string> &b)
		 {
			 if (a.first != b.first)
				 return a.first < b.first;
			 return a.second < b.second;
		 });
}

void PrintResults(const vector<pair<int, string>> &maxRankVector, ofstream &fv)
{
	for (auto itr = maxRankVector.begin(); itr != maxRankVector.end(); itr++)
	{
		fv << itr->first << " " << itr->second << endl;
	}
}

void InsertResult(vector<pair<int, string>> &maxRankVector, map<string, int> &rank)
{
	for (auto &ipair : rank)
	{
		if (ipair.second >= 0 && ipair.first != "__TOP_INPUT__")
		{
			maxRankVector.push_back(make_pair(ipair.second, ipair.first));
		}
	}
}
