#ifndef HCM_INST_PORT_H
#define HCM_INST_PORT_H

#include "hcmObject.h"


/**
 * A hcmInstPort is the instantiation of the master cell port on its instance.
 * hcmInstPort is a mutable object.
 */
class hcmInstPort : public hcmObject {
  // RepInvariant:
  	// (inst != null) && (connectedNode != null) && (connectedPort != null)

  // Abstraction Function:
    // hcmInstPort is an object that represents: 
    // a connection between a node(connectedNode) to a port(connectedPort) of a instance cell within the owner instance cell(inst).

  private:
    // ints - pointer to the instance cell this object is contained in 
    hcmInstance* inst;
    // connectedNode - pointer to the node this object is connected to
    hcmNode* connectedNode;
    // connectedPort - pointer to the port instance of the master cell.
    hcmPort* connectedPort;

  public:
    /** @fn hcmInstPort(hcmInstance* instance, hcmNode* node, hcmPort* port)
     * @brief hcmInstPort constractor.
     * @param instance - pointer to the instance cell this object is contained in 
     * @param node - pointer to the node this object is connected to
     * @param port - pointer to the port instance of the master cell
     * @return none
     * @throws invalid_argument if one of the pointers is null
     */
    hcmInstPort(hcmInstance* instance, hcmNode* node, hcmPort* port);

    /** @fn ~hcmInstPort()
     * @brief hcmInstPort distractor.
     * @return none
     */
    ~hcmInstPort();  

    /** @fn void printInfo()
     * @brief print information about this hcmInstPort.
     * @return none
     */
    void printInfo();

    /** @fn hcmNode *getNode() const
     * @brief gets a pointer to the connected node. this method doesn't change the state of the object
     * @return the pointer to ths connected node
     */
    hcmNode* getNode() const;

    /** @fn hcmPort *getPort() const
     * @brief gets a pointer to the port instance of the master cell. this method doesn't change the state of the object
     * @return the pointer to ths port instance of the master cell
     */
    hcmPort* getPort() const;

    /** @fn hcmInstance *getInst() const
     * @brief gets a pointer to the instance cell this object is contained in. this method doesn't change the state of the object
     * @return the instance cell this object is contained in 
     */
    hcmInstance* getInst() const;

    /** @fn void setConnectedNode(hcmNode *connectedNode)
     * @brief sets the pointer to the node this object is connected to.
     * @param connectedNode - the pointer to the node this object is connected to.
     * @return none
     * @throws invalid_argument if the pointer is null
     */
    void setConnectedNode(hcmNode *connectedNode);

    friend class hcmCell;
};

#endif
