#include <errno.h>
#include <signal.h>
#include <sstream>
#include <fstream>
#include "hcm.h"
#include "flat.h"
#include <list>

#define DEBUG 1
using namespace std;

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
		cerr << "Usage: " << argv[0] << "  [-v] \"<top cell name>\" file1.v [file2.v] ... \n";
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

	cout << "-I- Verilog parsing completed." << endl;

	/*direct to file*/
	string fileName = cellName + string(".stat");
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

	hcmCell *flatCell = hcmFlatten(cellName + string("_flat"), topCell, globalNodes);
	cout
		<< "-I- Top cell flattened" << endl;

	fv << "file name: " << fileName << endl;

	if (DEBUG)
		cout << "-I- Starting Design Analysis ..." << endl;

	/// === ANSWERS START HERE ===

	// Written by:
	//				Amir Zuabi 		 - 212606222
	//				Alexey Vasilayev - 323686683
	//
	// plz note: updates comments to fit the given pdf, since there were some changes in the questions wording.

	//  === Section A ===
	//	find all nodes in folded top module. excluding global nodes (VSS and VDD).
	//	(assign your answer for section a to topLevelNodeCounter)
	int topLevelNodeCounter = 0;

	// get node map
	map<string, hcmNode *> currentNodeMap = topCell->getNodes();

	// count & exclude global nodes
	for (auto it : currentNodeMap)
	{
		string nodeName = it.first;
		if (DEBUG && false)
			cout << "Node name: " << nodeName << endl;
		if (globalNodes.find(nodeName) == globalNodes.end())
		{
			topLevelNodeCounter++;
		}
	}

	// debug print of global nodes
	if (DEBUG && false)
	{
		// print global nodes
		cout << "Global nodes: ";
		for (auto gn : globalNodes)
		{
			cout << gn << " ";
		}
		cout << endl;
	}

	// debug prints
	if (DEBUG)
		cout << "total nodes: " << currentNodeMap.size() << endl;
	if (DEBUG)
		cout << "a: " << topLevelNodeCounter << endl;

	// file insertion
	fv << "a: " << topLevelNodeCounter << endl;

	//  === Section B ===
	//	find number of instances in folded top level cell.
	//	(assign your answer for section b to topLevelInstanceCounter)
	int topLevelInstanceCounter = 0;
	map<string, hcmInstance *> currentInstanceMap = topCell->getInstances();
	topLevelInstanceCounter = currentInstanceMap.size();

	fv << "b: " << topLevelInstanceCounter << endl;
	if (DEBUG)
		cout << "b: " << topLevelInstanceCounter << endl;

	//  === Section C ===
	//	How many instances of the cell “nand” exist in cells of the folded model?
	//	Don’t count “nand” instances that are contained within other instances.
	//	(assign your answer for section c to cellNameFoldedCounter)

	// the idea:
	//		1.loop over instances,
	//		2.check each instance's master (ignore duplicates)
	//		3.loop over master list, count nands in each master definition.

	// so basically we check all instances of "nand" that are in the definitions used by the design.

	int cellNameFoldedCounter = 0;
	vector<hcmCell *> mastersToVisit;

	// fetch all masters from instances
	for (auto it : currentInstanceMap)
	{
		hcmInstance *currentInstance = it.second;
		hcmCell *currentMasterCell = currentInstance->masterCell();

		// check if master already in list
		bool found = false;
		for (auto mc : mastersToVisit)
		{
			if (mc == currentMasterCell)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			mastersToVisit.push_back(currentMasterCell);
		}
	}

	// loop over master list and count nands
	for (auto mc : mastersToVisit)
	{
		map<string, hcmInstance *> masterInstanceMap = mc->getInstances();
		for (auto it : masterInstanceMap)
		{
			hcmInstance *currentInstance = it.second;
			hcmCell *currentMasterCell = currentInstance->masterCell();
			if (currentMasterCell->getName() == "nand") // should probably compare to nand instance name?
			{
				cellNameFoldedCounter++;
			}
		}
	}

	fv << "c: " << cellNameFoldedCounter << endl;
	if (DEBUG)
		cout << "c: " << cellNameFoldedCounter << endl;

	//  === Section D ===
	//	How many instances of cell “nand” exist in the entire hierarchy (means the number of “nand”s that are needed for full implementation of the top cell)?
	//	It is recommended to use here the flatmodel cell.
	//	(assign your answer for section d to cellNameFlatendCounter)
	int cellNameFlatendCounter = 0;

	// the idea:
	map<string, hcmInstance *> flatInstanceMap = flatCell->getInstances();
	for (auto it : flatInstanceMap)
	{
		hcmInstance *currentInstance = it.second;
		hcmCell *currentMasterCell = currentInstance->masterCell();
		if (currentMasterCell->getName() == "nand") // should probably compare to nand instance name?
		{
			cellNameFlatendCounter++;
		}
	}

	cout << "-I- Total instances in flat cell: " << flatInstanceMap.size() << endl;
	fv << "d: " << cellNameFlatendCounter << endl;
	if (DEBUG)
		cout << "d: " << cellNameFlatendCounter << endl;

	//  === Section E ===
	//	find the deepest reach of a top level node.
	//
	// 	How many levels of hierarchy traverse the top cell node with the deepest reach?
	//	Reach refers to the number of hierarchical levels a traversal moves through,
	//	starting from a specific node and continuing until it reaches a cell that contains no further instances.
	//
	//	For example, a top cell with no instances in it will have reach of 1.
	//	A cell with node connected to one instance will have reach of 2.
	//	(assign your answer for section e to deepestReach)

	int deepestReach = 1; // default is 1

	// this is a DFS problem, we will solve using a stack like we did in the tutorial.

	//	iterate over top-level nodes,
	//	follow only the instances actually connected to the node,
	//	and walk down until we cant anymore, then fold back up and try other route.

	for (auto nodePair : topCell->getNodes())
	{
		hcmNode *topNode = nodePair.second;

		// skip VDD/VSS
		if (globalNodes.find(topNode->getName()) != globalNodes.end())
		{
			continue;
		}

		// stack holds (node, depth). depth counts cells: top node is 1.
		vector<pair<hcmNode *, int>> stack;
		stack.push_back(make_pair(topNode, 1));

		while (!stack.empty())
		{
			// get top
			pair<hcmNode *, int> current = stack.back();
			stack.pop_back();

			hcmNode *node = current.first;
			int depth = current.second;

			// check if current is the deepest
			if (depth > deepestReach)
			{
				deepestReach = depth;
			}

			// follow instance ports connected to this node
			for (auto ipPair : node->getInstPorts())
			{
				hcmInstPort *ip = ipPair.second;
				hcmPort *masterPort = ip->getPort();
				if (!masterPort)
				{
					continue;
				}
				hcmNode *innerNode = masterPort->owner(); // corresponding node inside the master cell
				stack.push_back(make_pair(innerNode, depth + 1));
			}
		}
	}

	fv << "e: " << deepestReach << endl;
	if (DEBUG)
		cout << "e: " << deepestReach << endl;

	//  === Section F ===
	//	find hierarchical names of deepest reaching nodes.
	//	What are the hierarchical names of the deepest nodes, i.e. the nodes that are in the lowest cells levels.
	//	Order the node names lexicographically.
	//	assign your answer for section f to the given, What are the hierarchical names of the deepest nodes, i.e. the nodes that are in the lowest cells levels.
	//	Order the node names lexicographically. assign your answer for section f to the given
	//	(assign your answer for section f to listOfHierarchicalNameOfDeepestReachingNodes)
	list<string> listOfHierarchicalNameOfDeepestReachingNodes;
	//---------------------------------------------------------------------------------//
	// enter your code here
	//---------------------------------------------------------------------------------//
	for (auto it : listOfHierarchicalNameOfDeepestReachingNodes)
	{
		// erase the '/' in the beginning of the hierarchical name.
		// i.e. the name in listOfHierarchicalNameOfDeepestReachingNodes should be "/i1/i2/i3/i5/N1",
		// and the printed name should be "i1/i2/i3/i5/N1".
		it.erase(0, 1);
		fv << it << endl;
	}

	return (0);
}
