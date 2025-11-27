#include "hcm.h"
#include "auxiliary.h"

using namespace std;

const char* hcmPortDirNames[] =
  {
  stringify( in ),
  stringify( out ),
  stringify( in_out ),
  };

void hcmNode::printInfo(){
	string hasPort = "YES";
	if(port == NULL){
		hasPort = "NO";
	}
	cout << "\tNode: " + name + " ownerCell: " +cell->getName() + " has port: " + hasPort << endl;
	for(auto it = instPorts.begin(); it != instPorts.end(); ++it) {
				it->second->printInfo();
	}
}

hcmNode::hcmNode(string nodeName, hcmCell *ownerCell){
	name = nodeName;
	cell = ownerCell;
	port = NULL;
}

hcmNode::~hcmNode(){
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

	deletePort();
	cell->deleteNode(name);
}

hcmPort* hcmNode::createPort(hcmPortDir dir){
	//port = new hcmPort(name+'_'+hcmPortDirNames[dir], this,dir);
	port = new hcmPort(name, this,dir);
	return port;
}

hcmRes hcmNode::deletePort(){
	if(port == NULL) {
		return BAD_PARAM;
	}
	hcmPort* tmp = port;
	if(!(port->destructorCalled)){
		port = NULL;
		delete tmp;
	}
	port = NULL;
	return OK;
}

hcmPort* hcmNode::getPort(){
	return port;
}

hcmCell* hcmNode::owner(){
	return cell;
}

const hcmPort* hcmNode::getPort() const{
	return port;
}

bool hcmNode::connectPort(hcmInstPort* instPort){
	if(port == NULL){
		cout << "Cannot connect port in node: " + name + " because it doesn't have a port." << endl;
		return false;
	}
	port->instPorts[instPort->getName()] = instPort;
	return true;
}

bool hcmNode::disconnectPort(hcmInstPort* instPort){
	if(port == NULL){
		//cout << "Cannot disconnect port in node: " + name + " because it doesn't have a port." << endl;
		return false;
	}
	port->instPorts.erase(instPort->getName());
	return true;
}

map<string, hcmInstPort* > &hcmNode::getInstPorts(){
	return instPorts;
}

const map<string, hcmInstPort* > &hcmNode::getInstPorts() const{
	return instPorts;
}

