#ifndef HCM_CELL_H
#define HCM_CELL_H

#include "hcmObject.h"

/**
 * A hcmCell a single hierarchy of the design.
 * hcmCell is a mutable object.
 */
class hcmCell : public hcmObject {
  // RepInvariant:
  	//  for each cell in the cells container - hcmCell != NULL
    // a mapping between the name of a cell and the pointer to the hcmCell object.

  // Abstraction Function:
    //  design - pointer to the design this hcmCell is contained in, the owner.
    //  cells - a mapping between the name of a cell(e.g B_I1) and the pointer to the hcmInstance object (e.h Inst_I1{C})
    //  myInstances - a mapping of the hcmInstances that this cell contains.
    //  nodes - a mapping between a name of a node(e.g Node P1) to the hcmPort Object.
    //  buses - a mapping between a name of a port/bus(e.g Port P1) to a pair representing a range of the port(e.g P1[7:0]).
  private:
    //  design - pointer to the design this hcmCell is contained in, the owner.
    hcmDesign* design;

    // cells - container of tuples of type (string, hcmInstance*) - 
    // for each tuple, the string repersent the name of the cell(e.g A_I1),
    // and the hcmInstance* is a reference for the hcmInstance object contained in another cell.
    // for example for Cell B - we create the instance I1 in Cell A -> <A_I1, pointer to the hcmInstance of Cell B in Cell A>
    map< string, hcmInstance* > cells;

    // myInstances - container of tuples of type (string, hcmInstance*) - 
    // for each tuple, the string repersent the name of the cell(e.g B_I1),
    // and the hcmInstance* is a reference for the hcmInstance objects contianed in this cell.
    // for example for Cell B - we created the instance I1 of type Cell C -> <B_I1, pointer to the hcmInstance of type Cell C in Cell B>
    map< string, hcmInstance* > myInstances;

    // nodes - container of tuples of type (string, hcmNode*) - 
    // for each tuple, the string repersent the name of the node(e.g Node P1),
    // and the hcmNode* is a reference for the hcmNode object.
    map< string, hcmNode* > nodes;

    // buses - container of tuples of type (string, pair<int,int>) - 
    // for each tuple, the string repersent the name of the bus / port(e.g P1),
    // and the pair<int,int> representing a range of the port(e.g P1[7:0]).
    map< string, pair<int,int> > buses;

    /** @fn bool instPortParametersValid(hcmInstance *inst, hcmNode *node, hcmPort* port)
    * @brief a checker for a set of parameters .
    * @param inst - a pointer to hcmInstance to check.
    * @param node - a pointer to hcmNode to check.
    * @param port - a pointer to hcmPort to check.
    * @return true if the parmeters are valid\n false otherwise.
    * @throws 
    */
    bool instPortParametersValid(hcmInstance* inst, hcmNode* node, hcmPort* port);

  public:
  
    /** @fn hcmCell(string name, hcmDesign* d)
     * @brief hcmCell constractor.
     * @param name - the name of the created Cell
     * @param d - the name of the design this cell is created under
     * @return none
     */
    hcmCell(string name, hcmDesign* d); 

    /** @fn ~hcmCell()
     * @brief hcmCell distractor.
     * @return none
     */
    ~hcmCell();

    /** @fn void printInfo()
     * @brief print information about this object.
     * @return none
     */
    void printInfo();

    /** @fn hcmDesign* owner()
     * @brief gets the pointer to the containing design object.
     * @return the pointer to the containing design object.
     */
    hcmDesign* owner();

    /** @fn hcmInstance *createInst(string name, hcmCell* masterCell)
     * @brief creates and return a new hcmInstance of this Cell with the name \a name contained in \a masterCell.\n
     * the method updates the inner containers accordingly.
     * @param name - the name of the new hcmInstance.
     * @param masterCell - pointer to the typed cell the new instance is based on.
     * @return pointer for the new created hcmInstance.
     * @throws NullPointerException if one of the params is NULL
     */
    hcmInstance* createInst(string name, hcmCell* masterCell);

    /** @fn hcmInstance *createInst(string name, string masterCellName)
     * @brief creates and return a new hcmInstance of this Cell with the name \a name contained in \a masterCellName.\n
     * the method updates the inner containers accordingly.
     * @param name - the name of the new instance.
     * @param masterCellName - the name of the typed cell the new instance is based on.
     * @return pointer for the new created hcmInstance.
     * @throws NullPointerException if there is no cell with \a masterCellName in the current design.
     */
    hcmInstance* createInst(string name, string masterCellName);

    /** @fn hcmRes deleteInst(string name)
     * @brief .
     * @param name - 
     * @return none
     */
    hcmRes deleteInst(string name);

    /** @fn hcmNode *createNode(string name)
     * @brief creates and return a new hcmNode in this Cell with the name \a name.\n 
     * the method updates the inner containers accordingly.
     * @param name - the name of the new hcmNode.
     * @return pointer for the new created hcmNode.\n 
     * Null in case a node with the same name already exist in this cell.
     */
    hcmNode* createNode(string name);

    /** @fn hcmRes deleteNode(string name)
     * @brief .
     * @param name - the name of the created Cell
     * @return none
     */
    hcmRes deleteNode(string name);

    /** @fn void createBus(string name, int high , int low, hcmPortDir dir = NOT_PORT)
     * @brief creates a new bus(a sequence of bits - each bit will be represented by a new hcmNode and corresponding hcmPort).\n 
     * the bus will be with created with name \a name and range from \a low to \a high. 
     * the method updates the inner containers accordingly.
     * @param name - string repersenting the name of the new bus.
     * @param high - int repersenting upper bound of the range.
     * @param low - int repersenting lower bound of the range.
     * @param dir - hcmPortDir repersenting the direction of the bus 
     * @return none
     * @see hcmPortDir
     */
    void createBus(string name, int high, int low, hcmPortDir dir = NOT_PORT);
    
    /** @fn void deleteBus(string name)
     * @brief .
     * @param name - 
     * @return none
     */
    void deleteBus(string name);

    /** @fn hcmInstPort *connect(hcmInstance *inst, hcmNode *node, hcmPort* port)
     * @brief create and return a new hcmInstPort based on the \a port of the \a inst and connect it to \a node.\n
     * the method updates the inner containers accordingly.
     * @param inst - hcmInstance represent the instance to connect the \a node to.
     * \a inst should be part of this cell. 
     * @param node - hcmNode represent the node to be connected in this cell.
     * \a node should be part of this cell.
     * @param port - hcmPort represent a port in the \a inst to be connected to the \a node.
     * \a port should be part of \a inst.
     * @return pointer to the new hcmInstPort in case of valid parameters.\n Null otherwise.
     */
    hcmInstPort* connect(hcmInstance* inst, hcmNode* node, hcmPort* port);

    /** @fn hcmInstPort *connect(hcmInstance *inst, hcmNode *node, string portName)
     * @brief create and return a new hcmInstPort based on a port with \a portName of the \a inst and connect it to \a node.\n
     * the method updates the inner containers accordingly.
     * @param inst - hcmInstance represent the instance to connect the \a node to.
     * \a inst should be part of this cell. 
     * @param node - hcmNode represent the node to be connected in this cell.
     * \a node should be part of this cell.
     * @param portName - string represent a name of a port in the \a inst to be connected to the \a node.
     * there should be a port with \a portName as part of \a inst.
     * @return pointer to the new hcmInstPort in case of valid parameters.\n Null otherwise.
     */
    //Connect:
    // Returns NULL if it failed to connect
    // If the PortName is a bus, the function returns a RANDOM 
    // hcmInstPort with one of the bus' nodes.
    // Otherwise it returns the hcmInstPort that was created. 
    hcmInstPort* connect(hcmInstance* inst, hcmNode* node, string portName);

    /** @fn static hcmRes disConnect(hcmInstPort *instPort)
     * @brief .
     * @param instPort - 
     * @return none
     */
    static hcmRes disConnect(hcmInstPort* instPort); // equals to: delete instPort;

    /** @fn hcmInstance *getInst(string name)
     * @brief gets the hcmInstance with name \a name if exist in the current cell.
     * @param name - string represent the name of the desired hcmInstance.
     * @return hcmInstance with name \a name if exist in the current cell.\n Null otherwise.
     */
    hcmInstance* getInst(string name);

    /** @fn hcmNode *getNode(string name)
     * @brief gets the hcmNode with name \a name if exist in the current cell.
     * @param name - string represent the name of the desired hcmNode.
     * @return hcmNode with name \a name if exist in the current cell.\n Null otherwise.
     */
    hcmNode* getNode(string name);

    /** @fn hcmPort *getPort(string name)
     * @brief gets the hcmPort with name \a name if exist in the current cell.
     * @param name - string represent the name of the desired hcmPort.
     * @return hcmPort with name \a name if exist in the current cell.\n Null otherwise.
     */
    hcmPort* getPort(string name);

    /** @fn const hcmInstance *getInst(string name) const
     * @brief gets the hcmInstance with name \a name if exist in the current cell. this method doesn't change the state of the object.
     * @param name - string represent the name of the desired hcmInstance.
     * @return const hcmInstance with name \a name if exist in the current cell.\n Null otherwise.
     */
    const hcmInstance* getInst(string name) const;

    /** @fn const hcmNode *getNode(string name) const
     * @brief gets the hcmNode with name \a name if exist in the current cell. this method doesn't change the state of the object.
     * @param name - string represent the name of the desired hcmNode.
     * @return const hcmNode with name \a name if exist in the current cell.\n Null otherwise.
     */
    const hcmNode* getNode(string name) const;

    /** @fn const hcmPort *getPort(string name) const
     * @brief gets the hcmPort with name \a name if exist in the current cell. this method doesn't change the state of the object.
     * @param name - string represent the name of the desired hcmPort.
     * @return const hcmPort with name \a name if exist in the current cell.\n Null otherwise.
     */
    const hcmPort* getPort(string name) const;

    /** @fn vector<hcmPort*> getPorts()
     * @brief gets all the hcmPort's that exist in the current cell.
     * @return vector of hcmPort's that exist in the current cell.\n Null if there are no ports.
     */
    vector<hcmPort*> getPorts();

    /** @fn map< string, hcmInstance* > & getInstances()
     * @brief gets a container of tuples of type (string, hcmInstance*). \n
     * for each tuple, the string repersent the name of the cell(e.g A_I1), \n
     * and the hcmInstance* is a reference for the hcmInstance object contained in another cell. \n
     * for example for Cell B - we create the instance I1 in Cell A -> <A_I1, pointer to the hcmInstance of Cell B in Cell A> .
     * @return a map object as described above.
     */
    map< string, hcmInstance* >& getInstances();

    /** @fn const map< string, hcmInstance* > & getInstances() const
     * @brief gets a const container of tuples of type (string, hcmInstance*). this method doesn't change the state of the object. \n
     * for each tuple, the string repersent the name of the cell(e.g A_I1), \n
     * and the hcmInstance* is a reference for the hcmInstance object contained in another cell. \n
     * for example for Cell B - we create the instance I1 in Cell A -> <A_I1, pointer to the hcmInstance of Cell B in Cell A> .
     * @return a const map object as described above.
     */
    const map< string, hcmInstance* >& getInstances() const;

    /** @fn map< string, hcmInstance* > & getInstantiations()
     * @brief gets a container of tuples of type (string, hcmInstance*). \n
     * for each tuple, the string repersent the name of the cell(e.g B_I1),
     * and the hcmInstance* is a reference for the hcmInstance objects contianed in this cell.
     * for example for Cell B - we created the instance I1 of type Cell C -> <B_I1, pointer to the hcmInstance of type Cell C in Cell B>
     * @return a map object as described above.
     */
    map< string, hcmInstance* >& getInstantiations();

    /** @fn map< string, hcmNode* > & getNodes()
     * @brief gets a container of tuples of type (string, hcmNode*). \n
     * nodes - container of tuples of type (string, hcmNode*) - 
     * for each tuple, the string repersent the name of the node(e.g Node P1),
     * and the hcmNode* is a reference for the hcmNode object.
     * @return  a map object as described above.
     */
    map< string, hcmNode* >& getNodes();

    /** @fn const map< string, hcmNode* > & getNodes() const
     * @brief gets a container of tuples of type (string, hcmNode*). this method doesn't change the state of the object. \n
     * nodes - container of tuples of type (string, hcmNode*) - 
     * for each tuple, the string repersent the name of the node(e.g Node P1),
     * and the hcmNode* is a reference for the hcmNode object.
     * @return a map object as described above.
     */
    const map< string, hcmNode* >& getNodes() const;
    
    /** @fn const map< string, pair<int,int> >& getBuses() const
     * @brief gets a container of tuples of type (string, pair<int,int>). this method doesn't change the state of the object. \n
     * buses - container of tuples of type (string, pair<int,int>) - 
     * for each tuple, the string repersent the name of the bus / port(e.g P1),
     * and the pair<int,int> representing a range of the port(e.g P1[7:0]).
     * @return a map object as described above.
     */
    const map< string, pair<int,int> >& getBuses() const;

    friend class hcmDesign;

};


#endif
