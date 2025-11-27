#include "hcm.h"
#include <fstream>
#include <list>
#include <set>

using namespace std;

/**
 * hcmNodeCtx class represent a node and it's hcmInstance parents.
 * hcmNodeCtx is a mutable object.
 */
class hcmNodeCtx {
  private:
    // parentInsts - container of type const hcmInstance* - 
    // the hcmInstance repersent a instance ancestor of the node
    list<const hcmInstance*> parentInsts;
    // pointer to the node this context is describing 
    const hcmNode* node;
  public:
    /** @fn hcmNodeCtx(list<const hcmInstance*>& parentInsts, const hcmNode* node)
     * @brief constractor of hcmNodeCtx
     * @param parentInsts - refernce to list<const hcmInstance*>  
     * @param node - const pointer to a hcmNode
     * @return none
     */
    hcmNodeCtx(list<const hcmInstance*>& parentInsts_, const hcmNode* node_);

    /** @fn string getName() const
     * @brief gets the name of the context node. the name is a concatenation of all parentInsts and the current cell name.
     * @return string represantion of the context node name 
     */
    string getName() const;

    /** @fn list<const hcmInstance*>& getParents()
     * @brief gets parentInsts container
     * @return list of parentInsts
     */
    list<const hcmInstance*>& getParents() { return(parentInsts); };

    /** @fn const hcmNode* getNode() const
     * @brief gets the hcmNode of the hcmNodeCtx
     * @return hcmNode of the hcmNodeCtx
     */
    const hcmNode* getNode() const { return(node); };

    friend class cmpNodeCtx;
};

/**
 * cmpNodeCtx class - impelment the compare operation for hcmNodeCtx class
 */
class cmpNodeCtx {
  public:
    bool operator()(const hcmNodeCtx& a, const hcmNodeCtx& b) const {
      list<const hcmInstance*>::const_iterator apI = a.parentInsts.begin();
      list<const hcmInstance*>::const_iterator bpI = b.parentInsts.begin();
      while ((apI != a.parentInsts.end()) && (bpI != b.parentInsts.end())) {
        if ((*apI) != (*bpI)) {
          return ((*apI) < (*bpI));
        }
        apI++; bpI++;
      }

      if (apI != a.parentInsts.end()) {
        return false;
      } 

      if (bpI != b.parentInsts.end()) {
        return true;
      }

      return (a.node < b.node);
    };
};

/**
 * vcdFormatter class will genarte and mange the vcd file.
 * NOTE: the created VCD only contains top level nodes for nodes that are external to an instance. 
 * vcdFormatter is a mutable object.
 */
class vcdFormatter {
  private:
    // Output stream class to operate on.
    ofstream vcd;
    // true if the parser is OK, false otherwise
    bool is_good;
    // codeByNodeCtx - container of tuples of type (const hcmNodeCtx, string) - 
    // for each tuple, the hcmNodeCtx repersent the contex of the node,
    // and the string is a VCDId representation of the hcmNodeCtx.
    map<const hcmNodeCtx, string, cmpNodeCtx> codeByNodeCtx;
    // const pointer to hcmCell of the topCell to parse
    const hcmCell* topCell;
    // container for the global nodes
    set<string> glbNodeNames;
    // debug mode - true print all inside nodes, false - print only input / output to vcd file
    bool debug_mode;

    /** @fn int dfsVCDScope(list<const hcmInstance*>& parentInsts)
     * @brief recursive function to print the wires and module definitions to the vcd file
     * @param parentInsts - refernce to list<const hcmInstance*>
     * @return 0 on success
     */
    int dfsVCDScope(list<const hcmInstance*>& parentInsts);
    
    /** @fn string getVCDId(int id)
     * @brief get the string of the VCD code based on an integer 
     * @param id - the integer id
     * @return string of the VCD code based on an integer 
     */
    string getVCDId(int id);

    /** @fn int genVCDHeader()
     * @brief create the vcd header file 
     * @return  0 on success, 1 otherwise
     */
    int genVCDHeader();

  public:
    /** @fn vcdFormatter(string fileName, const hcmCell* cell, set<string>& glbNodeNames)
     * @brief constractor of vcdFormatter
     * @param fileName - name of the vcd file to generate
     * @param cell - const hcmCell* of the top cell
     * @param glbNodeNames - refernce to set<string> containing all the global nodes
     * @param debug_mode - true print all inside nodes, false - print only input / output
     * @return none
     */
    vcdFormatter(string fileName, const hcmCell* cell, set<string>& glbNodeNames_, bool debug_mode_ = false);

    /** @fn ~vcdFormatter()
     * @brief destructor of vcdFormatter
     * @return none
     */
    ~vcdFormatter();

    /** @fn bool good()
     * @brief gets the status of the parsing if successful or not.
     * @return true if the parser is OK, false otherwise
     */
    bool good() { return(is_good);};

    /** @fn int changeTime(unsigned long int newTime)
     * @brief add indication to the vcd file of an advance of one time unit 
     * @param newTime - the new time to advance to
     * @return 0 on success
     */
    int changeTime(unsigned long int newTime);

    /** @fn int changeValue(const hcmNodeCtx* nodeCtx, bool value)
     * @brief add indication to the vcd file of a change value to a wire represented by nodeCtx 
     * @param nodeCtx - const pointer to hcmNodeCtx representing a wire
     * @param value - new value of the wire
     * @return 0 on success, 1 otherwise
     */
    int changeValue(const hcmNodeCtx *nodeCtx, bool value);
};