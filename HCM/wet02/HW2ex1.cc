#include <errno.h>
#include <signal.h>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "hcm.h"
#include "flat.h"
#include "utils/System.h"
#include "utils/ParseUtils.h"
#include "utils/Options.h"
#include "core/Dimacs.h"
#include "core/Solver.h"

using namespace std;
using namespace Minisat;

// globals:
bool verbose = false;

///////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	int argIdx = 1;
	int anyErr = 0;
	unsigned int i;
	vector<string> specVlgFiles;
	vector<string> implementationVlgFiles;
	string specCellName;
	string implementationCellName;
	Solver solver;

	if (argc < 8)
	{
		anyErr++;
	}
	else
	{
		if (!strcmp(argv[argIdx], "-v"))
		{
			argIdx++;
			verbose = true;
		}
		if (!strcmp(argv[argIdx], "-s"))
		{
			argIdx++;
			specCellName = argv[argIdx++];
			while (strcmp(argv[argIdx], "-i"))
			{
				specVlgFiles.push_back(argv[argIdx++]);
			}
		}
		argIdx++;
		implementationCellName = argv[argIdx++];
		for (; argIdx < argc; argIdx++)
		{
			implementationVlgFiles.push_back(argv[argIdx]);
		}

		if (implementationVlgFiles.size() < 2 || specVlgFiles.size() < 2)
		{
			cerr << "-E- At least top-level and single verilog file required for spec model" << endl;
			anyErr++;
		}
	}

	if (anyErr)
	{
		cerr << "Usage: " << argv[0] << "  [-v] -s top-cell spec_file1.v spec_file2.v -i top-cell impl_file1.v impl_file2.v ... \n";
		exit(1);
	}

	string fileName = specCellName + ".cnf";
	set<string> globalNodes;
	globalNodes.insert("VDD");
	globalNodes.insert("VSS");

	// spec hcm
	hcmDesign *specDesign = new hcmDesign("specDesign");
	for (i = 0; i < specVlgFiles.size(); i++)
	{
		printf("-I- Parsing verilog %s ...\n", specVlgFiles[i].c_str());
		if (!specDesign->parseStructuralVerilog(specVlgFiles[i].c_str()))
		{
			cerr << "-E- Could not parse: " << specVlgFiles[i] << " aborting." << endl;
			exit(1);
		}
	}

	hcmCell *topSpecCell = specDesign->getCell(specCellName);
	if (!topSpecCell)
	{
		printf("-E- could not find cell %s\n", specCellName.c_str());
		exit(1);
	}

	hcmCell *flatSpecCell = hcmFlatten(specCellName + string("_flat"), topSpecCell, globalNodes);

	// implementation hcm
	hcmDesign *impDesign = new hcmDesign("impDesign");
	for (i = 0; i < implementationVlgFiles.size(); i++)
	{
		printf("-I- Parsing verilog %s ...\n", implementationVlgFiles[i].c_str());
		if (!impDesign->parseStructuralVerilog(implementationVlgFiles[i].c_str()))
		{
			cerr << "-E- Could not parse: " << implementationVlgFiles[i] << " aborting." << endl;
			exit(1);
		}
	}

	hcmCell *topImpCell = impDesign->getCell(implementationCellName);
	if (!topImpCell)
	{
		printf("-E- could not find cell %s\n", implementationCellName.c_str());
		exit(1);
	}

	hcmCell *flatImpCell = hcmFlatten(implementationCellName + string("_flat"), topImpCell, globalNodes);

	//---------------------------------------------------------------------------------//
	// enter your code below

	//---------------------------------------------------------------------------------//
	solver.toDimacs(fileName.c_str());
	solver.simplify();
	bool sat = solver.solve();
	if (sat)
	{
		cout << "SATISFIABLE!" << endl;
	}
	else
	{
		cout << "NOT SATISFIABLE!" << endl;
	}

	return 0;
}