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
	k = e.addKernel(0, "Fibonacci");
	k->second.insertNewConfigKeyValuePair("number", "10");

	k = e.addKernel(1, "Fibonacci");
	k->second.insertNewConfigKeyValuePair("number", "20");

	k = e.addKernel(2, "Fibonacci");
	k->second.insertNewConfigKeyValuePair("number", "30");

//	k = e.getKernelByID(0);
	// Add configurations
//	k->insertConfiguration(EPerfKernelConfiguration());

//	e.addKernel(3, "Fibonacci 40");

	cout << "Adding device\n";
	e.addDevice(0, "CPU");
	e.addDevice(1, "GPU");
	e.addDevice(2, "CPU + GPU");
	e.addSubDeviceToDevice(2, 0);
	e.addSubDeviceToDevice(2, 1);


	cout << "Generating timings and datavolumes\n";

	unsigned int f;
<<<<<<< HEAD
	for (int i = 0; i < 3; i++) {
		e.addKernelDataVolumes(1, 0, 1024, 512);
		e.startTimer(i, 0);
//		sleep(1);
=======
	for (int i = 0; i < 4; i++) {
		e.addKernelDataVolumes(i, 0, 4, 4);
		e.startTimer(i, 0);
>>>>>>> db
		f = fib((i + 1) * 10);
		e.stopTimer(i, 0);
		cout << "Fibonacci " << (i+1)*10 << ": " << f << "\n";
	}

	cout << "Printing content:\n";
	cout << e;

<<<<<<< HEAD
	// Write to JSON
//	e.exportToJSONFile("test.json");

	JSON<EPerfContainer> j("test.json");

	j.serialize(e);
=======
	cout << "Committing to DB\n";
	e.commitToDB();
>>>>>>> db

	return 0;

}
