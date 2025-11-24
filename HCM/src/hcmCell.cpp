#include "hcm.h"
#include "auxiliary.h"

extern string busNodeName(string busName, int index);

void hcmCell::printInfo(){
  cout << "------------------------" << endl;
  cout << "Cell " + name << endl;
  cout << "Cell " + name + "'s nodes:" << endl;
  for (auto it = nodes.begin(); it != nodes.end(); ++it) {
    it->second->printInfo();
  }
  cout << "Cell " + name + "'s cells:" << endl;
  for (auto it = cells.begin(); it != cells.end(); ++it) {
    it->second->printInfo();
  }
  cout << "Cell " + name + "'s instances:" << endl;
  for (auto it = myInstances.begin(); it != myInstances.end(); ++it) {
    it->second->printInfo();
  }
}

hcmCell::hcmCell(string cellName, hcmDesign* d) {
  design = d;
  this->name = cellName;
}

hcmCell::~hcmCell() {
  destructorCalled = true;
  design->deleteCell(name);


  cleanAndDestroy(nodes);
  cleanAndDestroy(cells);
  cleanAndDestroy(myInstances);
  // set<string> names;

  // delete the nodes map
	// for(auto it = nodes.begin(); it != nodes.end(); ++it) {
	// 	names.insert(it->first);
	// }
	// for(auto it = names.begin(); it != names.end() ; ++it){
	// 	hcmNode* elemToDelete = nodes[*it];
	// 	nodes.erase(*it);
  //   if (elemToDelete){
  //     delete elemToDelete;
  //   }
	// }
  // names.clear();

  // // delete the cells map
	// for(auto it = cells.begin(); it != cells.end(); ++it) {
	// 	names.insert(it->first);
	// }
	// for(auto it = names.begin(); it != names.end() ; ++it){
	// 	hcmInstance* elemToDelete = cells[*it];
	// 	cells.erase(*it);
  //   if (elemToDelete){
  //     delete elemToDelete;
  //   }	
  // }
  // names.clear();

  // // delete the myInstances map
	// for(auto it = myInstances.begin(); it != myInstances.end(); ++it) {
	// 	names.insert(it->first);
	// }
	// for(auto it = names.begin(); it != names.end() ; ++it){
	// 	hcmInstance* elemToDelete = myInstances[*it];
	// 	myInstances.erase(*it);
  //   if (elemToDelete){
  //     delete elemToDelete;
  //   }
	// }

  design = NULL;
}

hcmInstance* hcmCell::createInst(string name, hcmCell* masterCell){
  if(masterCell == NULL || cells.count(name) > 0){
    return NULL;
  }
  hcmInstance* instance = new hcmInstance(name,masterCell);
  instance->connectInstance(this);
  cells[name] = instance;
  masterCell->myInstances[name] = instance;
  return instance;
}

hcmInstance* hcmCell::createInst(string name, string masterCellName){
  if(cells.count(name) > 0){
    return NULL;
  }
  return createInst(name,design->getCell(masterCellName));
}


hcmRes hcmCell::deleteInst(string name){
  if(cells.count(name) == 0){
    return BAD_PARAM;
  }
  hcmInstance* inst = cells[name];
  if(!(inst->destructorCalled)){
    delete inst;
  } 
  else {
    //  cleanAndDestroy(inst->instPorts);
    inst->master->myInstances.erase(name); // equal? this->myInstances.erase(name) ???
    cells.erase(name);
    inst->cell = NULL;
    inst->master = NULL;
  }

  return OK;
}

hcmRes hcmCell::deleteNode(string name){
  if(nodes.count(name) == 0){
    return BAD_PARAM;
  }
  hcmNode* node = nodes[name];
  if(!(node->destructorCalled)){
    delete node;
  }
  nodes.erase(name);
  return OK;
}

hcmNode* hcmCell::createNode(string name){
  if(nodes.count(name) > 0 || buses.count(name) > 0) {
    cout << "Warning: Node: " + name + " already exists" << endl;
    return NULL;
  }
  hcmNode* node = new hcmNode(name,this);
  nodes[name] = node;
  return node;
}

hcmInstance* hcmCell::getInst(string name){
  if(cells.find(name) != cells.end()){
    return cells[name];
  } 
  else {
    return NULL;
  }
}

hcmNode* hcmCell::getNode(string name) {
  if(nodes.count(name) > 0) {
    return nodes[name];
  } 
  else {
    return NULL;
  }
}

hcmPort* hcmCell::getPort(string name) {
  if(nodes.count(name) > 0){
    return nodes[name]->getPort();
  } 
  else {
    return NULL;
  }
}

const hcmInstance* hcmCell::getInst(string name) const{
  if(cells.count(name) > 0){
    return cells.find(name)->second;
  } 
  else {
    return NULL;
  }
}

const hcmNode* hcmCell::getNode(string name) const{
  map< string, hcmNode* >::const_iterator nI = nodes.find(name);
  if(nI != nodes.end()){
    return (*nI).second;
  } 
  else {
    return NULL;
  }
}

const hcmPort* hcmCell::getPort(string name) const{
  const hcmNode *node = getNode(name);
  if(node) {
    return node->getPort();
  } 
  else {
    return NULL;
  }
}

bool hcmCell::instPortParametersValid(hcmInstance* inst, hcmNode* node, hcmPort* port){
  bool anyError = false;
  string instName = inst ? inst->getName() : "UNKNOWN";
  string nodeName = node ? node->getName() : "UNKNOWN";
  string portName = port ? port->getName() : "UNKNOWN";
  if (inst == NULL) {
    cout << "Error: the instance cannot be NULL while connecting inst port: " 
	  <<  portName << " to node: " << nodeName << endl;
    anyError = true;
  }
  if (node == NULL) {
    cout << "Error: the node cannot be NULL while connecting to instance: "
	  << instName << " port: " << portName << endl;
    anyError = true;
  }
  if (port == NULL) {
    cout << "Error: the port cannot be NULL while connecting  instance: " 
	  << instName << " to node:" << nodeName << endl;
    anyError = true;
  }

  if (anyError) 
    return false;

  if(nodes.count(node->name) == 0 || nodes[node->name] != node ){
    cout << "Error: " + node->name + "is not a node in the cell: " + name << endl;
    return false;
  }
  if(cells.count(inst->name) == 0 || cells[inst->name] != inst ){
    cout << "Error: " + inst->name + "is not an instance in the cell: " + name << endl;
    return false;
  }
  return true;
}

hcmInstPort* hcmCell::connect(hcmInstance* inst, hcmNode* node, hcmPort* port){
  if(!instPortParametersValid(inst,node,port)){
    return NULL;
  }
  hcmInstPort* instPort = new hcmInstPort(inst,node, port);
  node->instPorts[instPort->getName()] = instPort;
  inst->instPorts[instPort->getName()] = instPort;
  port->owner()->connectPort(instPort);
  return instPort;
}

hcmInstPort* hcmCell::connect(hcmInstance* inst, hcmNode* node, string portName){
  if(inst == NULL || node == NULL) {
    instPortParametersValid(inst,node,NULL);  // To print an error.
    return NULL;
  }
  hcmInstPort* res = NULL;
  vector<hcmPort*> availablePorts = inst->getAvailablePorts(portName);
  for(auto it = availablePorts.begin(); it!=availablePorts.end() ; it++) {
    res = connect(inst,node,*it); 
  }
  /*hcmNode* portOwner = NULL;
    for (map<string, hcmNode*>::iterator it = inst->master->nodes.begin(); it !=inst->master->nodes.end(); ++it) {
    if(it->second->getPort()->getName() == portName){
    portOwner = it->second;
    break;
    }
    }
    if(portOwner==NULL){
    cout << "The port: " + portName + " doesn't belong to any node at the instance's master cell!" << endl;
    return NULL;
    }*/
  return res;
}

hcmDesign* hcmCell::owner() {
  return design;
}

vector<hcmPort*> hcmCell::getPorts(){
  vector<hcmPort*> ports;
  hcmPort* port = NULL;
  for (auto it = nodes.begin(); it != nodes.end(); ++it) {
    port = it->second->getPort();
    if(port != NULL) {
      ports.push_back(port);
    }
  }
  return ports;
}

map< string, hcmInstance* >& hcmCell::getInstances()
{
  return cells;
}

const map< string, hcmInstance* >& hcmCell::getInstances() const
{
  return cells;
}

map< string, hcmInstance* >& hcmCell::getInstantiations(){
  return myInstances;
}

map< string, hcmNode* >& hcmCell::getNodes(){
  return nodes;
}

const map< string, hcmNode* >& hcmCell::getNodes() const{
  return nodes;
}

hcmRes hcmCell::disConnect(hcmInstPort* instPort){
  if(instPort == NULL){
    return BAD_PARAM;
  }
  if(!(instPort->destructorCalled)){
    delete instPort;
  } 
  else {
    string instPortName = instPort->name;
    instPort->connectedNode->instPorts.erase(instPortName);
    instPort->connectedNode = NULL;
    if (instPort->connectedPort->owner()) 
      instPort->connectedPort->owner()->disconnectPort(instPort);
    instPort->connectedPort = NULL;
    instPort->inst->instPorts.erase(instPortName);
    instPort->inst = NULL;
  }


  return OK;

}

void hcmCell::createBus(string name, int from , int to, hcmPortDir dir){
  if(from < 0 || to < 0) {
    cout << "Cannot add bus: " << name << ". Reason: Bad Parameters from: "
	  << from << " to: " << to << endl;
    return;
  }
  if(nodes.count(name)>0 || buses.count(name)>0) {
    cout << "Warning: Node: " + name + " already exists!" << endl;
    return;
  }
  int low = (from < to) ? from : to;
  int high = (from < to) ? to : from;

  for(int i = low ; i <= high; i++) {
    hcmNode* node = createNode(busNodeName(name,i));
    if(node == NULL) {
      cout << "Failed to create node: "+ name << '[' << i << ']' << endl;
      continue;
    }
    if (dir != NOT_PORT){
      node->createPort(dir);
    }
  }
  buses[name] = make_pair(from,to);
}

void hcmCell::deleteBus(string name){
  if(buses.count(name) == 0 ) {
    cout << "DeleteBus: bus " + name + " not found!" << endl;
    return;
  }
  for(int i = buses[name].second ; i<= buses[name].first ; i++ ) {
    deleteNode(busNodeName(name,i));
  }
  buses.erase(name);
}

const map< string, pair<int,int> >& hcmCell::getBuses() const{
  return buses;
}



