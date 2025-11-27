#include "hcm.h"

hcmObject::hcmObject(){
	destructorCalled = false;
}

hcmObject::~hcmObject(){
	destructorCalled = true;
	set<string> names;
	for(auto it = props.begin(); it != props.end(); ++it) {
		names.insert(it->first);
	}
	for(auto it = names.begin(); it != names.end() ; ++it){
		hcmProperty* elemToDelete = props[*it];
		props.erase(*it);
		if (elemToDelete){
			delete elemToDelete;
		}
	}
}

const string hcmObject::getName() const {
	return name;
}


/*hcmRes hcmObject::getProp(string name, string &s){

}

hcmRes hcmObject::getProp(string name, long int &i);
hcmRes hcmObject::getProp(string name, double &d);
// implicitly create new prop or update it
hcmRes hcmObject::setProp(string name, string s);
hcmRes hcmObject::setProp(string name, long int i);
hcmRes hcmObject::setProp(string name, double d);
hcmRes hcmObject::delProp(string name);*/
