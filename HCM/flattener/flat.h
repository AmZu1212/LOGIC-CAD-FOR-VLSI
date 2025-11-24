#ifndef __FLAT_H__
#define __FLAT_H__
#include "hcm.h"
#include <set>

using namespace std;

/** @fn hcmCell* hcmFlatten(string flatCellName, hcmCell* sCell, set<string>& globalNodes)
 * @brief create a flat model cell based on the given folded model. 
 * @param flatCellName - the name of the new flat cell
 * @param sCell - pointer to hcmCell represent the source cell
 * @param glbNodeNames - refernce to set<string> containing all the global nodes
 * @return pointer to the genereter flatten model on success, null otherwise
 */
hcmCell* hcmFlatten(string cellName, hcmCell* dCell, set<string>& globalNodes);

/** @fn int hcmWriteCellVerilog(hcmCell* topCell, string fileName)
 * @brief convert a hcmCell to a verilog file format.
 * @param topCell - pointer to hcmCell represent top cell
 * @param fileName - name of the file
 * @return 0 on success.
 */

int hcmWriteCellVerilog(hcmCell* topCell, string fileName);

#endif //__FLAT_H__
