#include "hcm.h"

void hcmDesign::printInfo(){
	cout << "Design " + name + " info:" <<endl;
	for(auto it = cells.begin(); it != cells.end(); ++it) {
		it->second->printInfo();
	}
	cout << "Done!" << endl;
}

hcmDesign::hcmDesign(string designName){
	name = designName;
}

hcmCell *hcmDesign::createCell(string name){
	if(cells.find(name) != cells.end()){
		cout << "Cell: " + name + " already exists in the design!" << endl;
		return NULL;
	}
	hcmCell* cell = new hcmCell(name,this);
	cell->createNode("VDD");
	cell->createNode("VSS");
	cells[name] =cell;
	return cell;
}

void hcmDesign::deleteCell(string name){
	if(cells.count(name) == 0 ) {
		return ;
	}
	
	hcmCell* cell = cells[name];
	if(!(cell->destructorCalled)) {
		delete cell;
	} 
	else {
		cells.erase(name);
	}
}

hcmCell *hcmDesign::getCell(string name){
	if(cells.count(name) == 0 ) {
			return NULL;
	}
	return cells[name];
}

hcmDesign::~hcmDesign(){
	set<string> names;
	for(auto it = cells.begin(); it != cells.end(); ++it) {
		names.insert(it->first);
	}
	for(auto it = names.begin(); it != names.end() ; ++it){
		hcmCell* elemToDelete = cells[*it];
		cells.erase(*it);
		if (elemToDelete){
      		delete elemToDelete;
    	}
	}
}

hcmRes hcmDesign::parseStructuralVerilog(const char *fileName){
	extern int read_verilog(hcmDesign* design, const char *fn);
	if (read_verilog(this,fileName)) {
	  return BAD_PARAM;
	}
	return OK;
}

