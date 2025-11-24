#include "hcm.h"

void hcmInstPort::printInfo(){
  cout << "\t\tInstPort: " +name; 
  if (connectedNode) 
    cout << " Node: " << connectedNode->getName();
  cout << endl;
}

hcmInstPort::hcmInstPort(hcmInstance* instance, hcmNode* node, hcmPort* port){
	inst = instance;
	connectedNode = node;
	connectedPort = port;
	// required to be unique name as node instPort is map by name !!!
	name = inst->getName()+'%'+port->getName();
}

hcmInstPort::~hcmInstPort(){
	destructorCalled = true;

	hcmCell::disConnect(this);
	inst = NULL;
	connectedNode = NULL;
	connectedPort = NULL;
}

hcmNode* hcmInstPort::getNode() const {
	return connectedNode;
}

hcmPort* hcmInstPort::getPort() const {
	return connectedPort;
}

hcmInstance* hcmInstPort::getInst() const {
	return inst;
}

void hcmInstPort::setConnectedNode(hcmNode *connectedNode) {
	this->connectedNode = connectedNode;
}