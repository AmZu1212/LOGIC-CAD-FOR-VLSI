#ifndef HCM_NODE_H
#define HCM_NODE_H

#include "hcmObject.h"


/**
 * A hcmNode is a connection of ports and instance ports
 * holds a container of the hcmInstPort.
 * hcmNode is a mutable object.
 */
class hcmNode : public hcmObject {
  // RepInvariant:
  	//  (cell != null)

  // Abstraction Function:
    // hcmPort is an object that represents: 
    // cell represent the owner cell this hcmNode is contained in.
    // port represent a pointer to the port connected to this hcmNode. 
    // port could be null is case there is not port connected to the hcmNode, meaning the node connect two InstPort.
    // instPorts is a collection of unbounded amount of elements of tuples of type (string, hcmInstPort*) - 
    // each tuple repersent a connection from the name (e.g CellA_I2/P7) to the instance of "hcmInstPort".

  private:
    // instPorts is a collection of unbounded amount of elements of tuples of type (string, hcmInstPort*) - 
    // each tuple repersent a connection from the name (e.g CellA_I2/P7) to the instance of "hcmInstPort".
    map< string , hcmInstPort* > instPorts;

    // port represent a pointer to the port connected to this hcmNode. 
    // port could be null is case there is not port connected to the node, meaning the node connect two InstPort.
    hcmPort* port;
    
    // cell represent the owner cell this hcmNode is contained in. cell can't be null.
    hcmCell* cell; 

    /** @fn bool connectPort(hcmInstPort* instPort)
     * @brief connect this hcmNode port to the given instPort. doesn't change this hcmNode instPorts
     * @param instPort - the instPort to connect to this hcmNode port instPorts
     * @return false in case this hcmNode does not connect to any port. true otherwise
     */
    bool connectPort(hcmInstPort* instPort);

    /** @fn bool disconnectPort(hcmInstPort* instPort);
     * @brief disconnect this hcmNode port to the given instPort. doesn't change this hcmNode instPorts
     * @param instPort - the instPort to disconnect from this hcmNode port instPorts
     * @return false in case this node does not connect to any port. true otherwise
     */
    bool disconnectPort(hcmInstPort* instPort);

  public:
    /** @fn hcmNode(string name, hcmCell* cell)
     * @brief hcmRes constractor.
     * @param name - the name of the created hcmNode.
     * @param cell - pointer to the hcmCell instance that this hcmNode is connected to. Can't be Null.
     * @return none
     * @throws invalid_argument if the cell pointer is null
     */
    hcmNode(string name, hcmCell* cell);
    
    /** @fn ~hcmNode()
     * @brief hcmNode distractor.
     * @return none
     */
    ~hcmNode();

    /** @fn void printInfo()
     * @brief print information about this port.
     * @return none
     */
    void printInfo();    

    /** @fn hcmCell* owner()
     * @brief gets the containing cell of this hcmNode.
     * @return none
     */
    hcmCell* owner();

    /** @fn hcmPort* createPort(hcmPortDir dir)
     * @brief create and return new instance of hcmPort, with the same name as this hcmNode, 
     * @param dir - the diraction for the created hcmPort to be initialize with
     * @return the created hcmPort
     */
    hcmPort* createPort(hcmPortDir dir);
    
    /** @fn hcmRes deletePort()
     * @brief delete the hcmPort was connected to this hcmNode
     * @return hcmRes enum indicate id the operetion succeeded
     */
    hcmRes deletePort();

    /** @fn hcmPort* getPort()
     * @brief gets a pointer to the connected port to this hcmNode
     * @return return a pointer to the connected port to this hcmNode
     */ 
    hcmPort* getPort();

    /** @fn const hcmPort* getPort() const
     * @brief gets a constant pointer to the connected port to this hcmNode. this method does not change the state of this hcmNode.
     * @return return a constant pointer to the connected port to this hcmNode
     */ 
    const hcmPort* getPort() const;

    /** @fn map<string, hcmInstPort* > &getInstPorts()
     * @brief gets a pointer to the InstPorts map. each element in the map repersent a connection from the name (e.g CellA_I2/P7) to the instance of "hcmInstPort".
     * @return return a pointer to the InstPorts map
     */ 
    map<string, hcmInstPort* >& getInstPorts();

    /** @fn const map<string, hcmInstPort* > &getInstPorts() const
     * @brief gets a const pointer to the InstPorts map. each element in the map repersent a connection from the name (e.g CellA_I2/P7) to the instance of "hcmInstPort".
     * @return return a const pointer to the InstPorts map
     */ 
    const map<string, hcmInstPort* >& getInstPorts() const;

    friend class hcmCell;
};


#endif
