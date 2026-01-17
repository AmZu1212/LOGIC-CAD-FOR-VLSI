#include <errno.h>
#include <signal.h>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "hcm.h"
#include "flat.h"
#include "utils/System.h"
#include "utils/ParseUtils.h"
#include "utils/Options.h"
#include "core/Dimacs.h"
#include "core/Solver.h"

using namespace std;
using namespace Minisat;

//globals:
bool verbose = false;

//=============================================================================
// Helper function to get or create a SAT variable for a node
//=============================================================================
Var getOrCreateVar(Solver& solver, map<string, Var>& nodeToVar, const string& nodeName) {
    if (nodeToVar.find(nodeName) == nodeToVar.end()) {
        Var v = solver.newVar();
        nodeToVar[nodeName] = v;
        if (nodeName.size() >= 3 && nodeName.compare(nodeName.size() - 3, 3, "VDD") == 0) {
            vec<Lit> clause;
            clause.clear();
            clause.push(mkLit(v));
            solver.addClause(clause);
        }
        if (nodeName.size() >= 3 && nodeName.compare(nodeName.size() - 3, 3, "VSS") == 0) {
            vec<Lit> clause;
            clause.clear();
            clause.push(~mkLit(v));
            solver.addClause(clause);
        }
        if (verbose) {
            cout << "Created var " << v << " for node: " << nodeName << endl;
        }
    }
    return nodeToVar[nodeName];
}

//=============================================================================
// CNF Encoding Functions for Gates using Tseytin Transformation
//=============================================================================

// BUFFER: Y = A
// Clauses: (A OR ~Y) AND (~A OR Y)
void encodeBuffer(Solver& solver, Var a, Var y) {
    vec<Lit> clause;
    // A OR ~Y
    clause.clear();
    clause.push(mkLit(a));
    clause.push(~mkLit(y));
    solver.addClause(clause);
    // ~A OR Y
    clause.clear();
    clause.push(~mkLit(a));
    clause.push(mkLit(y));
    solver.addClause(clause);
}

// INV/NOT: Y = ~A
// Clauses: (A OR Y) AND (~A OR ~Y)
void encodeInv(Solver& solver, Var a, Var y) {
    vec<Lit> clause;
    // A OR Y
    clause.clear();
    clause.push(mkLit(a));
    clause.push(mkLit(y));
    solver.addClause(clause);
    // ~A OR ~Y
    clause.clear();
    clause.push(~mkLit(a));
    clause.push(~mkLit(y));
    solver.addClause(clause);
}

// AND gate: Y = A1 AND A2 AND ... AND An
// Clauses: (~A1 OR ~A2 OR ... OR ~An OR Y) AND (Ai OR ~Y) for each i
void encodeAnd(Solver& solver, const vector<Var>& inputs, Var y) {
    vec<Lit> clause;
    // (~A1 OR ~A2 OR ... OR ~An OR Y)
    clause.clear();
    for (size_t i = 0; i < inputs.size(); i++) {
        clause.push(~mkLit(inputs[i]));
    }
    clause.push(mkLit(y));
    solver.addClause(clause);
    // (Ai OR ~Y) for each input
    for (size_t i = 0; i < inputs.size(); i++) {
        clause.clear();
        clause.push(mkLit(inputs[i]));
        clause.push(~mkLit(y));
        solver.addClause(clause);
    }
}

// NAND gate: Y = ~(A1 AND A2 AND ... AND An)
// Clauses: (~A1 OR ~A2 OR ... OR ~An OR ~Y) AND (Ai OR Y) for each i
void encodeNand(Solver& solver, const vector<Var>& inputs, Var y) {
    vec<Lit> clause;
    // (~A1 OR ~A2 OR ... OR ~An OR ~Y)
    clause.clear();
    for (size_t i = 0; i < inputs.size(); i++) {
        clause.push(~mkLit(inputs[i]));
    }
    clause.push(~mkLit(y));
    solver.addClause(clause);
    // (Ai OR Y) for each input
    for (size_t i = 0; i < inputs.size(); i++) {
        clause.clear();
        clause.push(mkLit(inputs[i]));
        clause.push(mkLit(y));
        solver.addClause(clause);
    }
}

// OR gate: Y = A1 OR A2 OR ... OR An
// Clauses: (A1 OR A2 OR ... OR An OR ~Y) AND (~Ai OR Y) for each i
void encodeOr(Solver& solver, const vector<Var>& inputs, Var y) {
    vec<Lit> clause;
    // (A1 OR A2 OR ... OR An OR ~Y)
    clause.clear();
    for (size_t i = 0; i < inputs.size(); i++) {
        clause.push(mkLit(inputs[i]));
    }
    clause.push(~mkLit(y));
    solver.addClause(clause);
    // (~Ai OR Y) for each input
    for (size_t i = 0; i < inputs.size(); i++) {
        clause.clear();
        clause.push(~mkLit(inputs[i]));
        clause.push(mkLit(y));
        solver.addClause(clause);
    }
}

// NOR gate: Y = ~(A1 OR A2 OR ... OR An)
// Clauses: (A1 OR A2 OR ... OR An OR Y) AND (~Ai OR ~Y) for each i
void encodeNor(Solver& solver, const vector<Var>& inputs, Var y) {
    vec<Lit> clause;
    // (A1 OR A2 OR ... OR An OR Y)
    clause.clear();
    for (size_t i = 0; i < inputs.size(); i++) {
        clause.push(mkLit(inputs[i]));
    }
    clause.push(mkLit(y));
    solver.addClause(clause);
    // (~Ai OR ~Y) for each input
    for (size_t i = 0; i < inputs.size(); i++) {
        clause.clear();
        clause.push(~mkLit(inputs[i]));
        clause.push(~mkLit(y));
        solver.addClause(clause);
    }
}

// XOR gate (2-input): Y = A XOR B
// Tseytin CNF: 4 clauses
// (~A OR ~B OR ~Y) AND (A OR B OR ~Y) AND (A OR ~B OR Y) AND (~A OR B OR Y)
void encodeXor2(Solver& solver, Var a, Var b, Var y) {
    vec<Lit> clause;
    // (~A OR ~B OR ~Y)
    clause.clear();
    clause.push(~mkLit(a));
    clause.push(~mkLit(b));
    clause.push(~mkLit(y));
    solver.addClause(clause);
    // (A OR B OR ~Y)
    clause.clear();
    clause.push(mkLit(a));
    clause.push(mkLit(b));
    clause.push(~mkLit(y));
    solver.addClause(clause);
    // (A OR ~B OR Y)
    clause.clear();
    clause.push(mkLit(a));
    clause.push(~mkLit(b));
    clause.push(mkLit(y));
    solver.addClause(clause);
    // (~A OR B OR Y)
    clause.clear();
    clause.push(~mkLit(a));
    clause.push(mkLit(b));
    clause.push(mkLit(y));
    solver.addClause(clause);
}

// DFF handling: For combinational equivalence, treat Q as free variable
// and D as output to be compared (or just ignore clock-based behavior)
// For this assignment, we'll treat DFF D input as pseudo-output and Q as pseudo-input

//=============================================================================
// Encode a single gate instance to CNF
//=============================================================================
void encodeGate(Solver& solver, hcmInstance* inst, map<string, Var>& nodeToVar,
                const string& prefix) {
    string gateName = inst->masterCell()->getName();

    // Get all instance ports
    map<string, hcmInstPort*> portMap;
    for (map<string, hcmInstPort*>::iterator it = inst->getInstPorts().begin();
         it != inst->getInstPorts().end(); ++it) {
        portMap[it->first] = it->second;
    }

    // Helper to get variable for a port
    // HCM names ports with hierarchical paths like "M1/F0_n1%A" where the actual port name is after '%'
    auto getPortVar = [&](const string& portName) -> Var {
        // First try direct lookup
        if (portMap.find(portName) != portMap.end()) {
            hcmNode* node = portMap[portName]->getNode();
            if (node) {
                return getOrCreateVar(solver, nodeToVar, prefix + node->getName());
            }
        }

        // If not found, search for port name after '%' separator
        string suffix = "%" + portName;
        for (map<string, hcmInstPort*>::iterator pit = portMap.begin();
             pit != portMap.end(); ++pit) {
            string fullName = pit->first;
            // Check if the port name ends with %<portName>
            if (fullName.length() >= suffix.length() &&
                fullName.compare(fullName.length() - suffix.length(), suffix.length(), suffix) == 0) {
                hcmNode* node = pit->second->getNode();
                if (node) {
                    return getOrCreateVar(solver, nodeToVar, prefix + node->getName());
                }
            }
        }

        // Still not found - print error with available ports
        cerr << "Error: Port " << portName << " not found in gate " << gateName << endl;
        cerr << "Available ports in this gate instance:" << endl;
        for (map<string, hcmInstPort*>::iterator pit = portMap.begin();
             pit != portMap.end(); ++pit) {
            cerr << "  - " << pit->first;
            if (pit->second->getNode()) {
                cerr << " (connected to node: " << pit->second->getNode()->getName() << ")";
            }
            cerr << endl;
        }
        exit(1);
    };

    // Handle different gate types
    if (gateName == "buffer") {
        Var a = getPortVar("A");
        Var y = getPortVar("Y");
        encodeBuffer(solver, a, y);
    }
    else if (gateName == "inv" || gateName == "not") {
        Var a = getPortVar("A");
        Var y = getPortVar("Y");
        encodeInv(solver, a, y);
    }
    else if (gateName == "and" || gateName == "and2") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        Var y = getPortVar("Y");
        encodeAnd(solver, inputs, y);
    }
    else if (gateName == "and3") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        Var y = getPortVar("Y");
        encodeAnd(solver, inputs, y);
    }
    else if (gateName == "and4") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        Var y = getPortVar("Y");
        encodeAnd(solver, inputs, y);
    }
    else if (gateName == "and5") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        Var y = getPortVar("Y");
        encodeAnd(solver, inputs, y);
    }
    else if (gateName == "and6") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        inputs.push_back(getPortVar("F"));
        Var y = getPortVar("Y");
        encodeAnd(solver, inputs, y);
    }
    else if (gateName == "and7") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        inputs.push_back(getPortVar("F"));
        inputs.push_back(getPortVar("G"));
        Var y = getPortVar("Y");
        encodeAnd(solver, inputs, y);
    }
    else if (gateName == "and8") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        inputs.push_back(getPortVar("F"));
        inputs.push_back(getPortVar("G"));
        inputs.push_back(getPortVar("H"));
        Var y = getPortVar("Y");
        encodeAnd(solver, inputs, y);
    }
    else if (gateName == "and9") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        inputs.push_back(getPortVar("F"));
        inputs.push_back(getPortVar("G"));
        inputs.push_back(getPortVar("H"));
        inputs.push_back(getPortVar("I"));
        Var y = getPortVar("Y");
        encodeAnd(solver, inputs, y);
    }
    else if (gateName == "nand" || gateName == "nand2") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        Var y = getPortVar("Y");
        encodeNand(solver, inputs, y);
    }
    else if (gateName == "nand3") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        Var y = getPortVar("Y");
        encodeNand(solver, inputs, y);
    }
    else if (gateName == "nand4") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        Var y = getPortVar("Y");
        encodeNand(solver, inputs, y);
    }
    else if (gateName == "nand5") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        Var y = getPortVar("Y");
        encodeNand(solver, inputs, y);
    }
    else if (gateName == "nand6") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        inputs.push_back(getPortVar("F"));
        Var y = getPortVar("Y");
        encodeNand(solver, inputs, y);
    }
    else if (gateName == "nand7") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        inputs.push_back(getPortVar("F"));
        inputs.push_back(getPortVar("G"));
        Var y = getPortVar("Y");
        encodeNand(solver, inputs, y);
    }
    else if (gateName == "nand8") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        inputs.push_back(getPortVar("F"));
        inputs.push_back(getPortVar("G"));
        inputs.push_back(getPortVar("H"));
        Var y = getPortVar("Y");
        encodeNand(solver, inputs, y);
    }
    else if (gateName == "nand9") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        inputs.push_back(getPortVar("F"));
        inputs.push_back(getPortVar("G"));
        inputs.push_back(getPortVar("H"));
        inputs.push_back(getPortVar("I"));
        Var y = getPortVar("Y");
        encodeNand(solver, inputs, y);
    }
    else if (gateName == "or" || gateName == "or2") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        Var y = getPortVar("Y");
        encodeOr(solver, inputs, y);
    }
    else if (gateName == "or3") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        Var y = getPortVar("Y");
        encodeOr(solver, inputs, y);
    }
    else if (gateName == "or4") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        Var y = getPortVar("Y");
        encodeOr(solver, inputs, y);
    }
    else if (gateName == "or5") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        Var y = getPortVar("Y");
        encodeOr(solver, inputs, y);
    }
    else if (gateName == "or6") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        inputs.push_back(getPortVar("F"));
        Var y = getPortVar("Y");
        encodeOr(solver, inputs, y);
    }
    else if (gateName == "or7") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        inputs.push_back(getPortVar("F"));
        inputs.push_back(getPortVar("G"));
        Var y = getPortVar("Y");
        encodeOr(solver, inputs, y);
    }
    else if (gateName == "or8") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        inputs.push_back(getPortVar("F"));
        inputs.push_back(getPortVar("G"));
        inputs.push_back(getPortVar("H"));
        Var y = getPortVar("Y");
        encodeOr(solver, inputs, y);
    }
    else if (gateName == "or9") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        inputs.push_back(getPortVar("F"));
        inputs.push_back(getPortVar("G"));
        inputs.push_back(getPortVar("H"));
        inputs.push_back(getPortVar("I"));
        Var y = getPortVar("Y");
        encodeOr(solver, inputs, y);
    }
    else if (gateName == "nor" || gateName == "nor2") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        Var y = getPortVar("Y");
        encodeNor(solver, inputs, y);
    }
    else if (gateName == "nor3") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        Var y = getPortVar("Y");
        encodeNor(solver, inputs, y);
    }
    else if (gateName == "nor4") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        Var y = getPortVar("Y");
        encodeNor(solver, inputs, y);
    }
    else if (gateName == "nor5") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        Var y = getPortVar("Y");
        encodeNor(solver, inputs, y);
    }
    else if (gateName == "nor6") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        inputs.push_back(getPortVar("F"));
        Var y = getPortVar("Y");
        encodeNor(solver, inputs, y);
    }
    else if (gateName == "nor7") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        inputs.push_back(getPortVar("F"));
        inputs.push_back(getPortVar("G"));
        Var y = getPortVar("Y");
        encodeNor(solver, inputs, y);
    }
    else if (gateName == "nor8") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        inputs.push_back(getPortVar("F"));
        inputs.push_back(getPortVar("G"));
        inputs.push_back(getPortVar("H"));
        Var y = getPortVar("Y");
        encodeNor(solver, inputs, y);
    }
    else if (gateName == "nor9") {
        vector<Var> inputs;
        inputs.push_back(getPortVar("A"));
        inputs.push_back(getPortVar("B"));
        inputs.push_back(getPortVar("C"));
        inputs.push_back(getPortVar("D"));
        inputs.push_back(getPortVar("E"));
        inputs.push_back(getPortVar("F"));
        inputs.push_back(getPortVar("G"));
        inputs.push_back(getPortVar("H"));
        inputs.push_back(getPortVar("I"));
        Var y = getPortVar("Y");
        encodeNor(solver, inputs, y);
    }
    else if (gateName == "xor2") {
        Var a = getPortVar("A");
        Var b = getPortVar("B");
        Var y = getPortVar("Y");
        encodeXor2(solver, a, b, y);
    }
    else if (gateName == "dff") {
        // For combinational equivalence checking:
        // Treat D as pseudo-output, Q as pseudo-input
        // We'll handle DFF matching separately
        if (verbose) {
            cout << "DFF found: " << inst->getName() << endl;
        }
    }
    else {
        cerr << "Warning: Unknown gate type: " << gateName << endl;
    }
}

//=============================================================================
// Encode entire circuit to CNF
//=============================================================================
void encodeCircuit(Solver& solver, hcmCell* flatCell, map<string, Var>& nodeToVar,
                   const string& prefix) {
    // Iterate over all instances in the flattened cell
    map<string, hcmInstance*>& instances = flatCell->getInstances();
    for (map<string, hcmInstance*>::iterator it = instances.begin();
         it != instances.end(); ++it) {
        hcmInstance* inst = it->second;
        encodeGate(solver, inst, nodeToVar, prefix);
    }
}

//=============================================================================
// Get Primary Inputs from a flattened cell
//=============================================================================
vector<string> getPrimaryInputs(hcmCell* flatCell) {
    vector<string> inputs;
    map<string, hcmNode*>& nodes = flatCell->getNodes();
    for (map<string, hcmNode*>::iterator it = nodes.begin();
         it != nodes.end(); ++it) {
        hcmNode* node = it->second;
        hcmPort* port = node->getPort();
        if (port && port->getDirection() == IN) {
            inputs.push_back(node->getName());
        }
    }
    return inputs;
}

//=============================================================================
// Get Primary Outputs from a flattened cell
//=============================================================================
vector<string> getPrimaryOutputs(hcmCell* flatCell) {
    vector<string> outputs;
    map<string, hcmNode*>& nodes = flatCell->getNodes();
    for (map<string, hcmNode*>::iterator it = nodes.begin();
         it != nodes.end(); ++it) {
        hcmNode* node = it->second;
        hcmPort* port = node->getPort();
        if (port && port->getDirection() == OUT) {
            outputs.push_back(node->getName());
        }
    }
    return outputs;
}

//=============================================================================
// Get DFFs from a flattened cell (returns map of DFF name to Q node name)
//=============================================================================
map<string, pair<string, string>> getDFFs(hcmCell* flatCell) {
    map<string, pair<string, string>> dffs; // name -> (D_node, Q_node)
    map<string, hcmInstance*>& instances = flatCell->getInstances();
    for (map<string, hcmInstance*>::iterator it = instances.begin();
         it != instances.end(); ++it) {
        hcmInstance* inst = it->second;
        if (inst->masterCell()->getName() == "dff") {
            string dNode = "", qNode = "";
            map<string, hcmInstPort*>& ports = inst->getInstPorts();
            for (map<string, hcmInstPort*>::iterator pit = ports.begin();
                 pit != ports.end(); ++pit) {
                const string& portName = pit->first;
                bool isD = (portName == "D") ||
                           (portName.size() > 2 && portName.compare(portName.size() - 2, 2, "%D") == 0);
                bool isQ = (portName == "Q") ||
                           (portName.size() > 2 && portName.compare(portName.size() - 2, 2, "%Q") == 0);
                if (isD && pit->second->getNode()) {
                    dNode = pit->second->getNode()->getName();
                }
                if (isQ && pit->second->getNode()) {
                    qNode = pit->second->getNode()->getName();
                }
            }
            dffs[inst->getName()] = make_pair(dNode, qNode);
        }
    }
    return dffs;
}

///////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
    int argIdx = 1;
    int anyErr = 0;
    unsigned int i;
    vector<string> specVlgFiles;
    vector<string> implementationVlgFiles;
    string specCellName;
    string implementationCellName;
    Solver solver;

    if (argc < 8) {
        anyErr++;
    }
    else {
        if (!strcmp(argv[argIdx], "-v")) {
            argIdx++;
            verbose = true;
        }
        if (!strcmp(argv[argIdx], "-s")) {
            argIdx++;
            specCellName = argv[argIdx++];
            while(strcmp(argv[argIdx], "-i")) {
                specVlgFiles.push_back(argv[argIdx++]);
            }
        }
        argIdx++;
        implementationCellName = argv[argIdx++];
        for (;argIdx < argc; argIdx++) {
            implementationVlgFiles.push_back(argv[argIdx]);
        }

        if (implementationVlgFiles.size() < 2 || specVlgFiles.size() < 2) {
            cerr << "-E- At least top-level and single verilog file required for spec model" << endl;
            anyErr++;
        }
    }

    if (anyErr) {
        cerr << "Usage: " << argv[0] << "  [-v] -s top-cell spec_file1.v spec_file2.v -i top-cell impl_file1.v impl_file2.v ... \n";
        exit(1);
    }

    string fileName = specCellName + ".cnf";
    set<string> globalNodes;
    globalNodes.insert("VDD");
    globalNodes.insert("VSS");

    // spec hcm
    hcmDesign* specDesign = new hcmDesign("specDesign");
    for (i = 0; i < specVlgFiles.size(); i++) {
        printf("-I- Parsing verilog %s ...\n", specVlgFiles[i].c_str());
        if (!specDesign->parseStructuralVerilog(specVlgFiles[i].c_str())) {
            cerr << "-E- Could not parse: " << specVlgFiles[i] << " aborting." << endl;
            exit(1);
        }
    }

    hcmCell *topSpecCell = specDesign->getCell(specCellName);
    if (!topSpecCell) {
        printf("-E- could not find cell %s\n", specCellName.c_str());
        exit(1);
    }

    hcmCell *flatSpecCell = hcmFlatten(specCellName + string("_flat"), topSpecCell, globalNodes);

    // implementation hcm
    hcmDesign* impDesign = new hcmDesign("impDesign");
    for (i = 0; i < implementationVlgFiles.size(); i++) {
        printf("-I- Parsing verilog %s ...\n", implementationVlgFiles[i].c_str());
        if (!impDesign->parseStructuralVerilog(implementationVlgFiles[i].c_str())) {
            cerr << "-E- Could not parse: " << implementationVlgFiles[i] << " aborting." << endl;
            exit(1);
        }
    }

    hcmCell *topImpCell = impDesign->getCell(implementationCellName);
    if (!topImpCell) {
        printf("-E- could not find cell %s\n", implementationCellName.c_str());
        exit(1);
    }

    hcmCell *flatImpCell = hcmFlatten(implementationCellName + string("_flat"), topImpCell, globalNodes);

    //---------------------------------------------------------------------------------//
    // FORMAL EQUIVALENCE VERIFICATION IMPLEMENTATION
    //---------------------------------------------------------------------------------//

    map<string, Var> nodeToVar;  // Global map for all nodes

    // Prefixes for spec and impl nodes (internal nodes need different variables)
    string specPrefix = "SPEC_";
    string impPrefix = "IMPL_";

    //---------------------------------------------------------------------------------//
    // Step 1: Get Primary Inputs/Outputs and match them
    //---------------------------------------------------------------------------------//
    vector<string> specPIs = getPrimaryInputs(flatSpecCell);
    vector<string> specPOs = getPrimaryOutputs(flatSpecCell);
    vector<string> impPIs = getPrimaryInputs(flatImpCell);
    vector<string> impPOs = getPrimaryOutputs(flatImpCell);

    if (verbose) {
        cout << "\n=== Spec Primary Inputs (" << specPIs.size() << ") ===" << endl;
        for (size_t i = 0; i < specPIs.size(); i++) {
            cout << "  " << specPIs[i] << endl;
        }
        cout << "\n=== Spec Primary Outputs (" << specPOs.size() << ") ===" << endl;
        for (size_t i = 0; i < specPOs.size(); i++) {
            cout << "  " << specPOs[i] << endl;
        }
    }

    //---------------------------------------------------------------------------------//
    // Step 2: Create SHARED variables for Primary Inputs (same input = same variable)
    //---------------------------------------------------------------------------------//
    map<string, Var> sharedInputVars;
    for (size_t i = 0; i < specPIs.size(); i++) {
        string piName = specPIs[i];
        Var v = solver.newVar();
        sharedInputVars[piName] = v;
        // Map both spec and impl versions to the same variable
        nodeToVar[specPrefix + piName] = v;
        nodeToVar[impPrefix + piName] = v;
        if (verbose) {
            cout << "Shared PI variable " << v << " for: " << piName << endl;
        }
    }

    //---------------------------------------------------------------------------------//
    // Step 3: Handle DFFs - match by name, share Q (pseudo-input), compare D (pseudo-output)
    //---------------------------------------------------------------------------------//
    map<string, pair<string, string>> specDFFs = getDFFs(flatSpecCell);
    map<string, pair<string, string>> impDFFs = getDFFs(flatImpCell);

    vector<pair<string, string>> dffDOutputPairs; // (spec_D, impl_D) to compare

    for (map<string, pair<string, string>>::iterator it = specDFFs.begin();
         it != specDFFs.end(); ++it) {
        string dffName = it->first;
        if (impDFFs.find(dffName) != impDFFs.end()) {
            // Found matching DFF
            string specD = it->second.first;
            string specQ = it->second.second;
            string impD = impDFFs[dffName].first;
            string impQ = impDFFs[dffName].second;

            // Share Q variables (pseudo-inputs)
            if (!specQ.empty() && !impQ.empty()) {
                Var qVar = solver.newVar();
                nodeToVar[specPrefix + specQ] = qVar;
                nodeToVar[impPrefix + impQ] = qVar;
                if (verbose) {
                    cout << "Shared DFF Q variable " << qVar << " for DFF: " << dffName << endl;
                }
            }

            // D outputs will be compared
            if (!specD.empty() && !impD.empty()) {
                dffDOutputPairs.push_back(make_pair(specPrefix + specD, impPrefix + impD));
            }
        }
    }

    //---------------------------------------------------------------------------------//
    // Step 4: Encode both circuits to CNF
    //---------------------------------------------------------------------------------//
    if (verbose) {
        cout << "\n=== Encoding Spec Circuit ===" << endl;
    }
    encodeCircuit(solver, flatSpecCell, nodeToVar, specPrefix);

    if (verbose) {
        cout << "\n=== Encoding Impl Circuit ===" << endl;
    }
    encodeCircuit(solver, flatImpCell, nodeToVar, impPrefix);

    //---------------------------------------------------------------------------------//
    // Step 5: Build Miter - XOR each pair of outputs, OR all XORs, assert TRUE
    //---------------------------------------------------------------------------------//
    vector<Var> xorOutputs;

    // Compare Primary Outputs
    for (size_t i = 0; i < specPOs.size(); i++) {
        string poName = specPOs[i];

        // Check if this PO exists in impl
        bool found = false;
        for (size_t j = 0; j < impPOs.size(); j++) {
            if (impPOs[j] == poName) {
                found = true;
                break;
            }
        }

        if (found) {
            Var specVar = getOrCreateVar(solver, nodeToVar, specPrefix + poName);
            Var impVar = getOrCreateVar(solver, nodeToVar, impPrefix + poName);

            // Create XOR output variable
            Var xorOut = solver.newVar();
            encodeXor2(solver, specVar, impVar, xorOut);
            xorOutputs.push_back(xorOut);

            if (verbose) {
                cout << "Comparing PO: " << poName << " (XOR var: " << xorOut << ")" << endl;
            }
        }
    }

    // Compare DFF D inputs
    for (size_t i = 0; i < dffDOutputPairs.size(); i++) {
        Var specVar = getOrCreateVar(solver, nodeToVar, dffDOutputPairs[i].first);
        Var impVar = getOrCreateVar(solver, nodeToVar, dffDOutputPairs[i].second);

        Var xorOut = solver.newVar();
        encodeXor2(solver, specVar, impVar, xorOut);
        xorOutputs.push_back(xorOut);

        if (verbose) {
            cout << "Comparing DFF D: " << dffDOutputPairs[i].first << " vs "
                 << dffDOutputPairs[i].second << " (XOR var: " << xorOut << ")" << endl;
        }
    }

    // OR all XOR outputs (miter output)
    // If any XOR is 1, the miter is 1 (outputs differ)
    // We want to find if miter can be 1 (SAT = not equivalent)
    if (xorOutputs.size() > 0) {
        // Add clause: (xor1 OR xor2 OR ... OR xorN)
        // This asserts that at least one output pair differs
        vec<Lit> miterClause;
        for (size_t i = 0; i < xorOutputs.size(); i++) {
            miterClause.push(mkLit(xorOutputs[i]));
        }
        solver.addClause(miterClause);
    }

    //---------------------------------------------------------------------------------//
    // Step 6: Write CNF file and solve
    //---------------------------------------------------------------------------------//
    solver.toDimacs(fileName.c_str());

    if (verbose) {
        cout << "\n=== Solver Statistics ===" << endl;
        cout << "Variables: " << solver.nVars() << endl;
        cout << "Clauses: " << solver.nClauses() << endl;
    }

    solver.simplify();
    bool sat = solver.solve();

    if (sat) {
        cout << "SATISFIABLE!" << endl;
        // cout << "The circuits are NOT EQUIVALENT." << endl;

        // Print counterexample - the input vector that causes mismatch
        // cout << "\nCounterexample (input values that cause mismatch):" << endl;
        // for (map<string, Var>::iterator it = sharedInputVars.begin();
        //      it != sharedInputVars.end(); ++it) {
        //     string inputName = it->first;
        //     Var v = it->second;
        //     if (solver.model[v] != l_Undef) {
        //         cout << "  " << inputName << " = "
        //              << (solver.model[v] == l_True ? "1" : "0") << endl;
        //     }
        // }

        // // Also show which outputs differ
        // cout << "\nOutput comparison:" << endl;
        // for (size_t i = 0; i < specPOs.size(); i++) {
        //     string poName = specPOs[i];
        //     if (nodeToVar.find(specPrefix + poName) != nodeToVar.end() &&
        //         nodeToVar.find(impPrefix + poName) != nodeToVar.end()) {
        //         Var specVar = nodeToVar[specPrefix + poName];
        //         Var impVar = nodeToVar[impPrefix + poName];
        //         if (solver.model[specVar] != l_Undef && solver.model[impVar] != l_Undef) {
        //             int specVal = (solver.model[specVar] == l_True) ? 1 : 0;
        //             int impVal = (solver.model[impVar] == l_True) ? 1 : 0;
        //             if (specVal != impVal) {
        //                 cout << "  " << poName << ": SPEC=" << specVal
        //                      << ", IMPL=" << impVal << " [MISMATCH]" << endl;
        //             }
        //         }
        //     }
        // }
    }
    else {
        cout << "NOT SATISFIABLE!" << endl;
        // cout << "The circuits are EQUIVALENT." << endl;
    }

    return 0;
}
