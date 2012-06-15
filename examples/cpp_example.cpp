#include "../include/eperf/EPerf.h"
#include "../include/Serialization/JSON.h"

#include <iostream>

using namespace ENHANCE;
using namespace std;

unsigned int fib(unsigned int n) {
	if (n < 2) {
		return n;
	} else {
		return fib(n-1) + fib(n-2);
	}
}

int main(void) {

	EPerf e;

	tKernelMap::iterator k;

	cout << "Adding kernel.\n";
	e.addKernel(0, "Fibonacci 10");
//	k->second.insertNewConfigKeyValuePair("number", "10");

	e.addKernel(1, "Fibonacci 20");
//	k->second.insertNewConfigKeyValuePair("number", "20");

	e.addKernel(2, "Fibonacci 30");
//	k->second.insertNewConfigKeyValuePair("number", "30");

//	k = e.getKernelByID(0);
	// Add configurations
//	k->insertConfiguration(EPerfKernelConfiguration());

	e.addKernel(3, "Fibonacci 40");

	cout << "Adding device\n";
	e.addDevice(0, "CPU");
	e.addDevice(1, "GPU");
	e.addDevice(2, "CPU + GPU");
	e.addSubDeviceToDevice(2, 0);
	e.addSubDeviceToDevice(2, 1);


	cout << "Generating timings and datavolumes\n";

	unsigned int f;
	for (int i = 0; i < 4; i++) {
//		e.addKernelConfigKeyValuePair(0, "number", "10");
		e.addKernelDataVolumes(i, 0, 4, 4);
		e.startTimer(i, 0);
		f = fib((i + 1) * 10);
		e.stopTimer(i, 0);
		cout << "Fibonacci " << (i+1)*10 << ": " << f << "\n";
	}

	cout << "Printing content:\n";
	cout << e;

	// Write to JSON
//	e.exportToJSONFile("test.json");

//	JSON<EPerfContainer> j("test.json");

//	j.serialize(e);

	return 0;

}
