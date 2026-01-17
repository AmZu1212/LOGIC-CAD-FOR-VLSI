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

	// Spec HCM routine
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

	// Impl HCM routine
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
	//
	/// === ANSWERS START HERE === ///
	// Written by:
	//				Amir Zuabi 		 - 212606222
	//				Alexey Vasilayev - 323686683

	// defines & helper functions (theres is alot, miniSAT building starts later)

	// Map each node name to a SAT variable (separate maps for spec and impl).
	map<string, int> specVarMap;
	map<string, int> implVarMap;

	// Primary input/output nodes keyed by name.
	map<string, hcmNode *> specPIs;
	map<string, hcmNode *> specPOs;
	map<string, hcmNode *> implPIs;
	map<string, hcmNode *> implPOs;

	// DFF bookkeeping: capture D/Q nodes to match by instance name.
	struct DffInfo
	{
		hcmNode *d; // input
		hcmNode *q; // output
		DffInfo() : d(NULL), q(NULL) {}
		DffInfo(hcmNode *dNode, hcmNode *qNode) : d(dNode), q(qNode) {}
	};

	map<string, DffInfo> specDffs;
	map<string, DffInfo> implDffs;

	// Helper to add a clause using Minisat's vec<Lit>.
	auto addClause = [&](const vector<Lit> &clause)
	{
		vec<Lit> lits;
		for (const auto &lit : clause)
		{
			lits.push(lit);
		}
		solver.addClause(lits);
	};

	// Create/get a SAT var for a node and force constants (VDD/VSS).
	auto getVar = [&](map<string, int> &varMap, hcmNode *node) -> int
	{
		string name = node->getName();
		auto it = varMap.find(name);
		if (it != varMap.end())
		{
			return it->second;
		}
		int v = solver.newVar();
		varMap[name] = v;
		if (name == "VDD")
		{
			addClause({mkLit(v)}); // force high
		}
		else if (name == "VSS")
		{
			addClause({~mkLit(v)}); // force low
		}
		return v;
	};

	// Add equivalence constraint a <-> b.
	auto addEq = [&](int a, int b)
	{
		addClause({~mkLit(a), mkLit(b)});
		addClause({mkLit(a), ~mkLit(b)});
	};

	// String prefix match utility.
	auto startsWith = [&](const string &s, const string &prefix) -> bool
	{
		return s.compare(0, prefix.size(), prefix) == 0;
	};

	// Collect PIs/POs from a flattened cell.
	auto collectPorts = [&](hcmCell *cell, map<string, hcmNode *> &pis, map<string, hcmNode *> &pos)
	{
		for (auto &it : cell->getNodes())
		{
			hcmNode *node = it.second;
			const hcmPort *port = node->getPort();
			if (!port)
			{
				continue; // if null skip
			}
			string name = node->getName();
			hcmPortDir dir = port->getDirection();
			if (dir == IN)
			{
				pis[name] = node;
			}
			else if (dir == OUT)
			{
				pos[name] = node;
			}
			else if (dir == IN_OUT) // *** check this wont make problems down the line
			{
				pis[name] = node;
				pos[name] = node;
			}
		}
	};

	// *** currently uses static names, maybe this is a bad idea. check stdcell later
	// Encode basic gate types into CNF using Tseytin clauses.
	auto encodeGate = [&](const string &gateName, const vector<int> &inputs, int outputVar)
	{
		if (gateName == "buffer")
		{
			if (inputs.size() != 1)
			{
				return;
			}
			int a = inputs[0];
			addClause({~mkLit(outputVar), mkLit(a)}); // a<->b cuz in == out (buffer)
			addClause({mkLit(outputVar), ~mkLit(a)});
			return;
		}
		if (gateName == "inv" || gateName == "not")
		{
			if (inputs.size() != 1)
			{
				return;
			}
			int a = inputs[0];
			addClause({~mkLit(outputVar), ~mkLit(a)}); // a -> not b (inverter)
			addClause({mkLit(outputVar), mkLit(a)});   // not a -> b
			return;
		}
		if (startsWith(gateName, "xor"))
		{
			if (inputs.size() != 2)
			{
				return;
			}
			int a = inputs[0];
			int b = inputs[1];
			addClause({~mkLit(a), ~mkLit(b), ~mkLit(outputVar)});
			addClause({mkLit(a), mkLit(b), ~mkLit(outputVar)});
			addClause({mkLit(a), ~mkLit(b), mkLit(outputVar)});
			addClause({~mkLit(a), mkLit(b), mkLit(outputVar)});
			return;
		}
		if (startsWith(gateName, "nand"))
		{
			for (size_t i = 0; i < inputs.size(); i++)
			{
				addClause({mkLit(outputVar), mkLit(inputs[i])});
			}
			vector<Lit> clause;
			clause.push_back(~mkLit(outputVar));
			for (size_t i = 0; i < inputs.size(); i++)
			{
				clause.push_back(~mkLit(inputs[i]));
			}
			addClause(clause);
			return;
		}
		if (startsWith(gateName, "nor"))
		{
			for (size_t i = 0; i < inputs.size(); i++)
			{
				addClause({~mkLit(outputVar), ~mkLit(inputs[i])});
			}
			vector<Lit> clause;
			clause.push_back(mkLit(outputVar));
			for (size_t i = 0; i < inputs.size(); i++)
			{
				clause.push_back(mkLit(inputs[i]));
			}
			addClause(clause);
			return;
		}
		if (gateName == "and" || (startsWith(gateName, "and") && !startsWith(gateName, "nand")))
		{
			for (size_t i = 0; i < inputs.size(); i++)
			{
				addClause({~mkLit(outputVar), mkLit(inputs[i])});
			}
			vector<Lit> clause;
			clause.push_back(mkLit(outputVar));
			for (size_t i = 0; i < inputs.size(); i++)
			{
				clause.push_back(~mkLit(inputs[i]));
			}
			addClause(clause);
			return;
		}
		// *** maybe decide on a better way to do this, strings are really not safe...
		if (gateName == "or" || (startsWith(gateName, "or") && !startsWith(gateName, "nor")))
		{
			for (size_t i = 0; i < inputs.size(); i++)
			{
				addClause({~mkLit(inputs[i]), mkLit(outputVar)});
			}
			vector<Lit> clause;
			clause.push_back(~mkLit(outputVar));
			for (size_t i = 0; i < inputs.size(); i++)
			{
				clause.push_back(mkLit(inputs[i]));
			}
			addClause(clause);
			return;
		}
	};

	// Encode a flattened cell: walk instances and add gate constraints.
	// DFFs are recorded for later matching instead of encoded directly.
	auto encodeCell = [&](hcmCell *cell, map<string, int> &varMap, map<string, DffInfo> &dffMap)
	{
		for (auto &instPair : cell->getInstances())
		{
			hcmInstance *inst = instPair.second;
			hcmCell *master = inst->masterCell();
			string gateName = master->getName();
			vector<int> inputs;
			vector<int> outputs;
			map<string, hcmNode *> portNodes;

			for (auto &portPair : inst->getInstPorts())
			{
				hcmInstPort *instPort = portPair.second;
				hcmPort *port = instPort->getPort();
				hcmNode *node = instPort->getNode();
				int v = getVar(varMap, node);
				portNodes[port->getName()] = node;
				if (port->getDirection() == IN)
				{
					inputs.push_back(v);
				}
				else if (port->getDirection() == OUT)
				{
					outputs.push_back(v);
				}
			}

			if (gateName == "dff")
			{
				hcmNode *dNode = NULL;
				hcmNode *qNode = NULL;
				if (portNodes.count("D"))
				{
					dNode = portNodes["D"];
					getVar(varMap, dNode);
				}
				if (portNodes.count("Q"))
				{
					qNode = portNodes["Q"];
					getVar(varMap, qNode);
				}
				dffMap[inst->getName()] = DffInfo(dNode, qNode);
				continue; // handeled later
			}

			if (outputs.empty())
			{
				continue; // skip dead ends (not sure this hits)
			}

			// run encode routine
			encodeGate(gateName, inputs, outputs[0]);
		}
	};



	// Real SAT building starts here...

	// Gather PIs/POs from both designs.
	collectPorts(flatSpecCell, specPIs, specPOs);
	collectPorts(flatImpCell, implPIs, implPOs);

	// Build CNF for both circuits.
	encodeCell(flatSpecCell, specVarMap, specDffs);
	encodeCell(flatImpCell, implVarMap, implDffs);

	// Constrain matching primary inputs to be equal.
	for (auto &piPair : specPIs)
	{
		const string &name = piPair.first;
		if (!implPIs.count(name))
		{
			continue;
		}
		// this is important. we match x in spec to x in impl
		int a = getVar(specVarMap, piPair.second);
		int b = getVar(implVarMap, implPIs[name]);
		addEq(a, b);
	}

	// Collect outputs to compare (spec vs impl).
	vector<pair<int, int>> comparePairs;
	for (auto &poPair : specPOs)
	{
		const string &name = poPair.first;
		if (!implPOs.count(name))
		{
			continue;
		}
		// same as the inputs...
		int a = getVar(specVarMap, poPair.second);
		int b = getVar(implVarMap, implPOs[name]);
		comparePairs.push_back(make_pair(a, b));
	}

	// DFF routine, same idea as the input/outputs. we match the outs and ins.
	// Match DFFs by name: tie Q (same state), compare D as outputs.
	for (auto &dffPair : specDffs)
	{
		const string &name = dffPair.first;
		if (!implDffs.count(name))
		{
			continue;
		}
		DffInfo specInfo = dffPair.second;
		DffInfo implInfo = implDffs[name];
		if (specInfo.q && implInfo.q)
		{
			int a = getVar(specVarMap, specInfo.q);
			int b = getVar(implVarMap, implInfo.q);
			addEq(a, b);
		}
		if (specInfo.d && implInfo.d)
		{
			int a = getVar(specVarMap, specInfo.d);
			int b = getVar(implVarMap, implInfo.d);
			comparePairs.push_back(make_pair(a, b));
		}
	}

	// Build XOR miter for each compared pair and OR them together.
	vector<Lit> mismatchLits;
	for (size_t idx = 0; idx < comparePairs.size(); idx++)
	{
		int a = comparePairs[idx].first;
		int b = comparePairs[idx].second;
		int z = solver.newVar();
		addClause({~mkLit(a), ~mkLit(b), ~mkLit(z)});
		addClause({mkLit(a), mkLit(b), ~mkLit(z)});
		addClause({mkLit(a), ~mkLit(b), mkLit(z)});
		addClause({~mkLit(a), mkLit(b), mkLit(z)});
		mismatchLits.push_back(mkLit(z));
	}

	if (!mismatchLits.empty())
	{
		// Enforce existence of at least one mismatch.
		addClause(mismatchLits);
	}

	//---------------------------------------------------------------------------------//
	solver.toDimacs(fileName.c_str());
	solver.simplify();
	bool sat = solver.solve();
	if (sat)
	{
		cout << "SATISFIABLE!" << endl;
		// Print one counterexample input vector (PI assignments).
		cout << "Counterexample (PIs):" << endl;
		for (auto &piPair : specPIs)
		{
			const string &name = piPair.first;
			int v = getVar(specVarMap, piPair.second);
			if (v >= 0 && v < solver.nVars() && solver.model[v] != l_Undef)
			{
				cout << "  " << name << " = " << (solver.model[v] == l_True ? 1 : 0) << endl;
			}
			else
			{
				cout << "  " << name << " = X" << endl;
			}
		}
	}
	else
	{
		cout << "NOT SATISFIABLE!" << endl;
	}

	return 0;
}
