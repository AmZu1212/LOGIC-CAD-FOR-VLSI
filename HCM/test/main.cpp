#include "hcm.h"
using namespace std;

void testParsing() {
	hcmDesign* d = new hcmDesign("MyDesign");
	d->parseStructuralVerilog("myrisc.v");
	d->printInfo();
	delete d;
}

int main(int argc, char* argv[]) {
	testParsing();
	return 0;
}


