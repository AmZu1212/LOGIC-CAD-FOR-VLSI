#ifndef HCM_TYPED_PROPERTY_H
#define HCM_TYPED_PROPERTY_H

#include "hcm.h"

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

  private:
    /** @fn hcmTypedProperty()
     * @brief hcmTypedProperty constractor.
     * @return None
     * @throws None
     */
    hcmTypedProperty(){} 

  public:
    map<string, T > values;

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

#endif