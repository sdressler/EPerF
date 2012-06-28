#include "../include/EPerf/EPerf.h"

#include <iostream>
#include <sstream>

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
	e.addKernel(0, "Fibonacci");
/*	e.addKernel(1, "Fibonacci");
	e.addKernel(2, "Fibonacci");
	e.addKernel(3, "Fibonacci 40");
*/
	cout << "Adding device\n";
	e.addDevice(0, "CPU");
	e.addDevice(1, "GPU");
	e.addDevice(2, "CPU + GPU");
	e.addSubDeviceToDevice(2, 0);
	e.addSubDeviceToDevice(2, 1);


	cout << "Generating timings and datavolumes\n";

	unsigned int f;
    for (int n = 0; n < 1; n++) {
	for (int i = 0; i < 4; i++) {

        EPerfKernelConf c;
		
        f = (i + 1) * 10;

        std::stringstream ss;
        ss << f;
        c.insertKernelConfPair("number", ss.str());

        e.setKernelConf(0, c);

//		e.addKernelDataVolumes(i, 0, 4, 4);
		e.startTimer(0, 0);
		f = fib(f);
		e.stopTimer(0, 0);
		cout << "Fibonacci " << (i+1)*10 << ": " << f << "\n";
	}
    }

	cout << "Printing content:\n";
	cout << e;

	cout << "Committing to DB\n";
	e.commitToDB();

	return 0;

}
