#ifndef HCM_H
#define HCM_H

// #include "hcm_common.h"
#include <map>
#include <set>

using namespace std;

/**
 *  Prototype class that represent an optional property.
 */
class hcmProperty {
  public:
    /** @fn ~hcmProperty()
     * @brief virtual distractor.
     * @return none
     * @throws 
     */
    virtual ~hcmProperty(){};
};

/**
 *  Template class that represent a typed property.
 *  property can be added to an hcmObject by the user.
 *  example for typedproperty : delay, tag, volume, etc.
 */
template <typename T>
class hcmTypedProperty: public hcmProperty {
  // RepInvariant:
  	//  none 

  // Abstraction Function:
    //  hcmTypedProperty is an object the represents a typed property.

  public:
    /** @fn hcmTypedProperty()
     * @brief hcmTypedProperty constractor.
     * @return None
     * @throws None
     */
    hcmTypedProperty(){}

    map<string, T > values;

    /** @fn ~hcmTypedProperty()
     * @brief hcmTypedProperty distractor.
     * @return None
     * @throws None
     */
    ~hcmTypedProperty(){
      	for (auto it = values.begin(); it != values.end(); ++it) {
          T* valueToDelete = get(it->first); 
          remove(it->first);
          delete valueToDelete;
	      }
    } 

    /** @fn T* get(string key)
     * @brief gets the value Type T that is represented by key
     * @param key - key of type string 
     * @return pointer to the value Type T that is represented by key\n
     * NULL if the key is not found
     * @throws None
     */
    T* get(string key) {
      if(values.count(key)) {
        return &values[key];
      }
      return NULL;
    }

    /** @fn void add(string name, T value)
     * @brief adds the value Type T with key represention
     * @param key - key of type string 
     * @param value - value of type T
     * @return None
     * @throws None
     */
    void add(string name, T value) {
      values[name] = value;
    }

    /** @fn void remove(string key)
     * @brief removes the value Type T with key represention
     * @param key - key of type string 
     * @return None
     * @throws None
     */
    void remove(string key) {
      values.erase(key);
    }
};

#include "hcmObject.h"
#include "hcmInstPort.h"
#include "hcmPort.h"
#include "hcmNode.h"
#include "hcmInstance.h"
#include "hcmCell.h"
#include "hcmDesign.h"

#endif
