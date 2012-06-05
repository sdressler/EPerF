#include "../include/eperf/EPerf.h"

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

	cout << "Adding kernel.\n";
	e.addKernel(0, "Fibonacci 10");
	e.addKernel(1, "Fibonacci 20");
	e.addKernel(2, "Fibonacci 30");
	e.addKernel(3, "Fibonacci 40");

	cout << "Adding device\n";
	e.addDevice(0, "Test Device");

	cout << "Generating timings and datavolumes\n";

	unsigned int f;
	for (int i = 0; i < 4; i++) {
		e.addKernelDataVolumes(1, 0, 1024, 512);
		e.startTimer(i, 0);
		sleep(1);
		f = fib((i + 1) * 10);
		e.stopTimer(i, 0);
		cout << "Fibonacci " << (i+1)*10 << ": " << f << "\n";
	}

	cout << "Printing content:\n";
	cout << e;

	return 0;

}
