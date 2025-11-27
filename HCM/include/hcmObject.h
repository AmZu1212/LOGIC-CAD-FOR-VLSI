#ifndef HCM_OBJECT_H
#define HCM_OBJECT_H

#include "hcm_common.h"

/**
 * A hcmObject is prototype for all classes to inheritance from
 * holds a container of the object properties and the object name.
 * hcmObject is a mutable object.
 */
class hcmObject {

  // RepInvariant:
  	// (name != null) 

  // Abstraction Function: 
    // name repersent the name of this object                                 
    // destructorCalled is a flag represent is the distractor was called 

  private:
    // props - container of tuples of type (string, hcmProperty*) - 
    // for each tuple, the string repersent the name of the template class (e.g DelayClass) that hcmProperty* is built with,
    // and the hcmProperty* is a reference for the hcmProperty class was build with that type corresponds to the name.
    map< string, hcmProperty* > props;

    // propNameToType - container of tuples of type (string, string) - 
    // for each tuple, the first string represent the name of the property (e.g Delay), 
    // and the second string represent the name of the type class this property is build from (e.g DelayClass).
    map< string, string > propNameToType;
    
  protected:
    // name repersent the name of this object                                 
    string name;
    // destructorCalled is a flag represent is the distractor was called 
    bool destructorCalled;

  public:
    /** @fn hcmObject()
     * @brief constractor of hcmObject.
     * @return none
     */
    hcmObject();
    
    /** @fn ~hcmObject()
     * @brief hcmObject distractor.
     * @return none
     */
    ~hcmObject();
    
    /** @fn const string getName() const
     * @brief gets the name of this hcmObject.
     * @return a constant string represent the name of this hcmObject.
     */
    const string getName() const;

    /** @fn hcmRes getProp(string name, T& value)
     * @brief tamplate method - finds the property if exist ,
     *        and insert into the given parmter "value" the value of the typed property
     * @param name - name of type string, which represents the wanted property to get
     * @param value - reference for value of type T to contain the desire value. 
     * @return OK if property was found\n
     * NOT_FOUND in case type hcmObject has no typedProperty T.
     */
    template <typename T>
    hcmRes getProp(string name, T& value) {
      // typeid(T).name() - built-in function that gets the name of the class T (e.g DelayClass).
      if(props.count(typeid(T).name())) {
        hcmTypedProperty<T>* typedProp = dynamic_cast<hcmTypedProperty<T>*>(props[typeid(T).name()]);
        if(typedProp == NULL) {
          cout << "ERROR: NULL TYPED PROP" << endl;
          return NOT_FOUND;
        }
        T* r = typedProp->get(name);
        if(r!= NULL) {
          value = *r;
          return OK;
        }
      }
      return NOT_FOUND;
    }

    /** @fn hcmRes setProp(string name, T value)
     * @brief Create and add hcmTypedProperty.
     * @param name - name of the new hcmTypedProperty.
     * @param value - class type of the new hcmTypedProperty.
     * @return OK in case we were able to add the new hcmTypedProperty.\n 
     * PROPERTY_EXISTS_WITH_DIFFERENT_TYPE - in case the name is found with diffrent typeName.
     */
    template <typename T>
    hcmRes setProp(string name, T value) {
      string typeName = typeid(T).name();
      if(propNameToType.count(name) && propNameToType[name]!=typeName ) {
      return PROPERTY_EXISTS_WITH_DIFFERENT_TYPE;
      }
      if(props.count(typeName)==0) {
      props[typeName] = new hcmTypedProperty<T>();
      }
      hcmProperty* prop = props[typeName];
      hcmTypedProperty<T>* typedProp = dynamic_cast<hcmTypedProperty<T>*>(prop);
      typedProp->add(name,value);
      propNameToType[name] = typeName;
      return OK;
    }

    /** @fn hcmRes delProp(string name)
     * @brief remove the hcmTypedProperty represented by name.
     * @param name - the name of the hcmTypedProperty.
     * @return OK in case we removed the hcmTypedProperty successfully.\n 
     * NOT_FOUND in case the hcmTypedProperty was not found.\n 
     * PROPERTY_EXISTS_WITH_DIFFERENT_TYPE in case the name is found with diffrent typeName.
     */
    template <typename T>
    hcmRes delProp(string name) {
      if(propNameToType.count(name) == 0) {
      return NOT_FOUND;
      }
      if(propNameToType[name] != typeid(T).name()) {
      return PROPERTY_EXISTS_WITH_DIFFERENT_TYPE;
      }
      hcmTypedProperty<T>* typedProp = 
      dynamic_cast<hcmTypedProperty<T>*>(props[propNameToType[name]]);
      typedProp->remove(name);
      return OK;
    }

};

#endif