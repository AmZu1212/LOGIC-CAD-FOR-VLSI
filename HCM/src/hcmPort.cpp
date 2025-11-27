#include "hcm.h"

void hcmPort::printInfo(){}

hcmPort::hcmPort(string portName, hcmNode* ownerNode , hcmPortDir direction){
	name = portName;
	dir = direction;
	node = ownerNode;
}

hcmPort::~hcmPort() {
	destructorCalled = true;

	set<string> names;
	// delete the instPorts map
	for(auto it = instPorts.begin(); it != instPorts.end(); ++it) {
		names.insert(it->first);
	}
	for(auto it = names.begin(); it != names.end() ; ++it){
		hcmInstPort* elemToDelete = instPorts[*it];
		instPorts.erase(*it);
		if (elemToDelete){
      		delete elemToDelete;
    	}
	}

	node->deletePort();
	node = NULL;
}

hcmNode* hcmPort::owner(){
	return node;
}

hcmPortDir hcmPort::getDirection() const{
	return dir;
}
