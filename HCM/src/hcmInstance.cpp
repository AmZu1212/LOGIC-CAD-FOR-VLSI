#include "hcm.h"

bool startsWith(string source, string prefix) {
  return source.substr(0,prefix.length()) == prefix;
}

void hcmInstance::printInfo(){
  cout << "\tInstance " + name + " owner: " + cell->getName() + " master: " + master->getName() << endl;
  for(auto it = instPorts.begin(); it != instPorts.end(); ++it) {
    it->second->printInfo();
  }
}

hcmInstance::hcmInstance(string instanceName, hcmCell* masterCell){
  name = instanceName;
  master = masterCell;
  cell = NULL;
}

hcmInstance::~hcmInstance(){
  destructorCalled = true;
  cell->deleteInst(name);

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

  master = NULL;
  cell = NULL;
}

void hcmInstance::connectInstance(hcmCell* containingCell){
  if(cell != NULL){
    return ;
  }
  cell = containingCell;
}

hcmCell* hcmInstance::owner(){
  return cell;
}

hcmCell* hcmInstance::masterCell(){
  return master;
}

vector<hcmPort*> hcmInstance::getAvailablePorts(){
  vector<hcmPort*> allPorts = master->getPorts();
  vector<hcmPort*> availablePorts;
  for(auto it = allPorts.begin(); it != allPorts.end() ; it++) {
    bool found = false;
    for(auto ipit = instPorts.begin(); ipit != instPorts.end(); ++ipit) {
      if((*it)->getName() == ipit->second->getPort()->getName()) {
        found = true;
        break;
      }
    }
    if(!found) { 
      availablePorts.push_back(*it); 
    }
  }
  return availablePorts;
}

vector<hcmPort*> hcmInstance::getAvailablePorts(string nodeName){
  vector<hcmPort*> allPorts = master->getPorts();
  vector<hcmPort*> availablePorts;

  // if the node name is a known bus
  map< string, pair<int,int> > buses = master->getBuses();
  if (buses.find(nodeName) != buses.end()) {
    int from = buses[nodeName].first;
    int to = buses[nodeName].second;
    // loop over all bus nodes and add to available port if they are ports...
    if (from <= to) {
      for (int i = from; i <= to; i++) {
	      hcmPort *port = master->getPort(busNodeName(nodeName, i));
	      if (port) 
	        availablePorts.push_back(port);
      }
    }
    else {
      for (int i = from; i >= to; i--) {
        hcmPort *port = master->getPort(busNodeName(nodeName, i));
        if (port) 
          availablePorts.push_back(port);
      }
    }
  } 
  else {
    hcmPort *port = master->getPort(nodeName);
    if (port) {
      availablePorts.push_back(port);
    }
  }

  // making sure not ALREADY CONNECTED
  for(auto it = availablePorts.begin(); it != availablePorts.end() ; it++) {
    for(auto ipit = instPorts.begin(); ipit != instPorts.end(); ++ipit) {
      if((*it)->getName() == ipit->second->getPort()->getName()) {
        //This bus is in use!
        availablePorts.clear();
        return availablePorts;
      }
    }
  }
  return availablePorts;
}

hcmInstPort* hcmInstance::getInstPort(string name)
{
  auto iI = instPorts.find(name);
  if (iI == instPorts.end()) {
    return NULL;
  } else {
    return (*iI).second;
  }
}

const hcmInstPort* hcmInstance::getInstPort(string name) const
{
  map< string , hcmInstPort *>::const_iterator iI = instPorts.find(name);
  if (iI == instPorts.end()) {
    return NULL;
  } else {
    return (*iI).second;
  }
}

map<string, hcmInstPort* >& hcmInstance::getInstPorts()
{
  return instPorts;
}

const map<string, hcmInstPort* >& hcmInstance::getInstPorts() const
{
  return instPorts;
}

const hcmCell* hcmInstance::masterCell() const { 
  return master; 
}