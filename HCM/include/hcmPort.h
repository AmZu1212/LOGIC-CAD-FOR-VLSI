#ifndef HCM_PORT_H
#define HCM_PORT_H

#include "hcmObject.h"

/**
 * A hcmPort is the connection to the cell nodes from outside.
 * holds a container of the hcmInstPort.
 * hcmPort is a mutable object.
 */
class hcmPort : public hcmObject {
  // RepInvariant:
  	//  (node != null) 

  // Abstraction Function:
    // hcmPort is an object that represents: 
    // a collection of unbounded amount of instPorts - each instPort is a "type" of this port.
    // for example, Cell C have port named P7.
    // if cell B contains two instantiations of cell C in it, then cell B will have instPort I1/P7 and I2/P7.
    // the node is the "wire" connected to this port

  private:
    // instPorts - container of tuples of type (string, hcmInstPort*) - 
    // each tuple repersent a connection from the name (e.g CellA_I2/P7) to the instance of "hcmInstPort".
    map< string , hcmInstPort*> instPorts;
    // node - a pointer to the "wire" this port is connected to. 
    hcmNode* node;
    // dir - a enum representing the direction of this port.
    hcmPortDir dir;

  public:
    /** @fn hcmRes delProp(string name)
     * @brief hcmRes constractor.
     * @param portName - the name of the created port.
     * @param ownerNode - pointer to the Node instance that is connected to this hcmPort. Can't be Null.
     * @param direction - the direction of this port.
     * @return none
     * @throws invalid_argument if the ownerNode pointer is null
     */
    hcmPort(string portName, hcmNode* ownerNode, hcmPortDir direction); 

    /** @fn ~hcmPort()
     * @brief hcmPort distractor.
     * @return none
     */
    ~hcmPort();

    /** @fn void printInfo()
     * @brief print information about this port.
     * @return none
     */
    void printInfo();

    /** @fn hcmPortDir getDirection() const
     * @brief gets the direction of the port. this method does not change the state of this port.
     * @return none
     */
    hcmPortDir getDirection() const;

    /** @fn hcmNode* owner() 
     * @brief gets a pointer to the hcmNode object this instace is connected to.
     * @return return a pointer to the hcmNode object this instace is connected to.
     */
    hcmNode* owner();

    friend class hcmNode;
};


#endif
