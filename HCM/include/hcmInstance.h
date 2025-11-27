#ifndef HCM_INSTANCE_H
#define HCM_INSTANCE_H

#include "hcmObject.h"


/**
 * A hcmInstance is the instantiation of another cell within the cell.
 * hcmInstance is a mutable object.
 */
class hcmInstance : public hcmObject {
  private:
    // RepInvariant:
    	//  for each instPort in the instPorts container - hcmInstPort != NULL &&
      //  master != NULL &&
      //  cell   != NULL

    // Abstraction Function:
      // instPorts - a mapping between the name of a instPort and the pointer to the hcmInstPort object.
      // master - type of this instance.
      // cell - the cell this instance is contained in, the owner.

    // instPorts - a mapping between the name of a instPort and the pointer to the hcmInstPort object
    map< string , hcmInstPort *> instPorts;
    // master - type of this instance (e.g AND2, NOR3...).
    hcmCell* master;
    // cell - the cell this instance is contained in, the owner (e.g Instance of AND2 is under Full-Adder cell).
    hcmCell* cell;

    /** @fn void connectInstance(hcmCell* cell)
     * @brief connect this instance to the given cell.
     * @param cell - pointer to the hcmCell object this instace is part of
     * @return none
     * @throws NullPointerException if the param is NULL
     */
    void connectInstance(hcmCell* cell);

  public:

    /** @fn hcmInstance(string instanceName, hcmCell* masterCell)
     * @brief constractor of hcmInstance .
     * @param instanceName - the name of the created hcmInstance
     * @param masterCell - pointer to the cell this instance represent
     * @return none
     * @throws NullPointerException if one of the params is NULL
     */
    hcmInstance(string instanceName, hcmCell* masterCell);

    /** @fn ~hcmInstance()
     * @brief distractor of hcmInstance.
     * @return none
     */
    ~hcmInstance();

    /** @fn void printInfo()
     * @brief print information about this object.
     * @return none
     */
    void printInfo();

    /** @fn hcmCell* owner()
     * @brief gets the pointer to the hcmCell object this instace is part of.
     * @return a pointer to the hcmCell object this instace is part of.\n
     * NULL - if this instance is not connected yet.
     */
    hcmCell* owner();  

    /** @fn hcmCell* masterCell()
     * @brief gets the pointer to the hcmCell object this instace represent.
     * @return a pointer to the hcmCell object this instace represent.
     */
    hcmCell* masterCell(); //Type of cell

    /** @fn const hcmCell* masterCell() const
     * @brief gets the const pointer to the hcmCell object this instace represent.
     * @return a const pointer to the hcmCell object this instace represent.
     */
    const hcmCell* masterCell() const;

    /** @fn vector<hcmPort*> getAvailablePorts()
     * @brief go over all the ports of the master cell and return which ports are not connected yet at this instance.
     * @return vector of all available hcmPorts 
     */
    vector<hcmPort*> getAvailablePorts();

    /** @fn vector<hcmPort*> getAvailablePorts(string nodeName)
     * @brief check if this port(bus) is available and return it 
     * @return vector the available port or ports if it is a bus\n 
     * in case the port or part of the bus is in use return an empty vector
     */
    vector<hcmPort*> getAvailablePorts(string nodeName); //For returning bus ports.

    /** @fn  map<string, hcmInstPort* > &getInstPorts() 
     * @brief gets a pointer to the map container of the hcmInstPorts
     * @return a pointer to the map container of the hcmInstPorts
     */
    map<string, hcmInstPort* >& getInstPorts();

    /** @fn const map<string, hcmInstPort* > &getInstPorts() const
     * @brief gets a const pointer to the map container of the hcmInstPorts
     * @return a const pointer to the map container of the hcmInstPorts
     */
    const map<string, hcmInstPort* >& getInstPorts() const;

    /** @fn hcmInstPort* getInstPort(string name) 
     * @brief gets a pointer of the port with identifier name
     * @param name - the name identifier of the desired port
     * @return pointer of the port with identifier name
     * @throws NullPointerException if the param is NULL
     */
    hcmInstPort* getInstPort(string name);

    /** @fn const hcmInstPort* getInstPort(string name) const
     * @brief gets a const pointer of the port with identifier name
     * @param name - the name identifier of the desired port
     * @return const pointer of the port with identifier name
     * @throws NullPointerException if the param is NULL
     */
    const hcmInstPort* getInstPort(string name) const;

    friend class hcmCell;
};


#endif
