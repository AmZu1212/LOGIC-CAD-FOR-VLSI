#ifndef HCM_DESIGN_H
#define HCM_DESIGN_H

#include "hcmObject.h"

/**
 * A hcmDesign is a container to hold a set of cells.
 * 
 * hcmDesign is a mutable object.
 */
class hcmDesign : public hcmObject {

  // RepInvariant:
  	//  for each cell in the cells container - hcmCell != NULL

  // Abstraction Function:
    //  cells - a mapping between the name of a cell and the pointer to the hcmCell object.
  private:
    map< string, class hcmCell* > cells;

  public:

    /** @fn hcmDesign(string name)
     * @brief constractor of hcmDesign.
     * @param name - the name of the created design.
     * @return none 
     */
    hcmDesign(string name);
  
    /** @fn ~hcmDesign()
     * @brief distractor of hcmDesign.
     * @return none
     */
    ~hcmDesign();
  
    /** @fn hcmCell *createCell(string name)
     * @brief create a mutable hcmCell.
     * @param name - the name of the created cell, need to be unique.
     * @return pointer to the created cell if successful.\n
     *         Null if a hcm cell with the same name exists. 
     * @throws
     */
    hcmCell* createCell(string name);
  
    /** @fn void deleteCell(string name)
     * @brief delete the hcmCell with the corresponding name from the design.\n
     * if there is no cell with this name do nothing.
     * @param name - the name of the deleted cell.
     * @return none
     */
    void deleteCell(string name);
  
    /** @fn hcmCell *getCell(string name)
     * @brief return a pointer to a hcmCell with the corresponding name.
     * @param name - the name of the wanted cell. 
     * @return pointer to hcmCell with the argument name\n
     *         Null if a hcm cell with the same name exists. 
     * @throws 
     */
    hcmCell* getCell(string name);
  
    /** @fn void printInfo()
     * @brief print information about this object.
     * @return none
     */
    void printInfo();
  
    /** @fn hcmRes parseStructuralVerilog(const char *fileName)
     * @brief Parse Verilog file in to a design object.
     * @param fileName - the name of the verilog file to be parsed
     * @return OK if the operation was successfull\n
     * BAD_PARAM if the parmeter supplied to the function is not valid
     */
    hcmRes parseStructuralVerilog(const char *fileName);
};


#endif
