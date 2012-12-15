#include <iostream>
#include "../include/EPerf/EPerf.h"

using namespace ENHANCE;

int main(void) {

	EPerf e("eperf.db", "Multidevice");

	// Add two devices
	e.addDevice(0, "CPU");
	e.addDevice(1, "GPU");
	e.addDevice(2, "CPU + GPU");

	// Add subdevices
	e.addSubDeviceToDevice(2, 0);
	e.addSubDeviceToDevice(2, 1);

	// Print
	std::cout << e << "\n";

    e.commitToDB();

	return 0;
}
