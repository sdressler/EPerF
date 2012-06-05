/**
 * Implementation of EPerf C++ Interface
 *
 * @author	Sebastian Dressler (ZIB)
 * @date	2012-05-24
 *
 * */

#include <stdexcept>

#include "../include/eperf/EPerf.h"
#include "../include/eperf/EPerfC.h"

//using namespace ENHANCE;

namespace ENHANCE {
void EPerf::checkDeviceExistance(int ID) {
	if (devices.find(ID) == devices.end()) {
		throw std::invalid_argument("Device ID not valid.");
	}
}

void EPerf::checkKernelExistance(int ID) {
	if (kernels.find(ID) == kernels.end()) {
		throw std::invalid_argument("Kernel ID not valid.");
	}
}

// Add a new kernel with unique ID and optional kernel name
void EPerf::addKernel(int ID, const std::string &kName) {
	// Try to insert, if it already exists -> exception
	std::pair<std::map<int, std::string>::iterator, bool> r = kernels.insert(
		std::pair<int, std::string>(ID, kName)
	);

	// Check for overwrite attempt
	if (r.second == false) {
		throw std::runtime_error("Kernel already exists.");
	}
}

// Add a new device with unique ID and optional device name
void EPerf::addDevice(int ID, const std::string &dName) {
	// Try to insert
	std::pair<std::map<int, std::string>::iterator, bool> r = devices.insert(
		std::pair<int, std::string>(ID, dName)
	);

	// Check for overwrite attempt
	if (r.second == false) {
		throw std::runtime_error("Device already exists.");
	}
}
/*
// Set the name of a kernel
void EPerf::setKernelName(int ID, const std::string &kName) {
	// Check if ID is valid
	checkKernelExistance(ID);
	// Write
	kernels[ID] = kName;
}

// Set the name of a device
void EPerf::setDeviceName(int ID, const std::string &dName) {
	// Check if ID is valid
	checkDeviceExistance(ID);
	// Write
	devices[ID] = dName;
}
*/

// Take a TimeSpec structure and convert it to double, seconds
double EPerf::convTimeSpecToDoubleSeconds(const struct timespec &t) {
	double tt = (double)t.tv_nsec * 1.0e-9;
	tt += (double)t.tv_sec;

	return tt;
}

// Start the time measurement of a specific kernel
void EPerf::startTimer(int KernelID, int DeviceID) {

	checkKernelExistance(KernelID);
	checkDeviceExistance(DeviceID);

	// Create entries in temp timespec
	struct timespec dummy;
	struct timespec *CPU_t;
	struct timespec *WCLK_t;

	std::pair<int, int> ref = std::pair<int, int>(KernelID, DeviceID);

	ttimes[ref].push_back(dummy);
	ttimes[ref].push_back(dummy);

	CPU_t = &ttimes[ref][0];
	WCLK_t = &ttimes[ref][1];

	// Save the timestamps
	clock_gettime(CPU_clockid, CPU_t);
	clock_gettime(WCLK_clockid, WCLK_t);

};

// Stop the time measurement and save the measured time
void EPerf::stopTimer(int KernelID, int DeviceID) {

	// Stop the time
	struct timespec t[2];
	clock_gettime(CPU_clockid, &t[0]);
	clock_gettime(WCLK_clockid, &t[1]);

	checkKernelExistance(KernelID);
	checkDeviceExistance(DeviceID);

	// Try to locate the start time
	std::map<std::pair<int, int>, std::vector<struct timespec> >::iterator timeit = ttimes.find(
		std::pair<int, int>(KernelID, DeviceID)
	);
	if (timeit == ttimes.end()) {
		throw std::runtime_error("Start time not found.");
	}

	// Convert the start time to double
	std::pair<int, int> ref = std::pair<int, int>(KernelID, DeviceID);
	
	double CPU_start = convTimeSpecToDoubleSeconds(ttimes[ref][0]);
	double CPU_stop  = convTimeSpecToDoubleSeconds(t[0]);
	
	double WCLK_start = convTimeSpecToDoubleSeconds(ttimes[ref][1]);
	double WCLK_stop = convTimeSpecToDoubleSeconds(t[1]);

	// Calculate the difference
	double CPU_d = CPU_stop - CPU_start;
	double WCLK_d = WCLK_stop - WCLK_start;

	// Check if it's negative (should not be the case) 
	if ((CPU_d < 0.0) || (WCLK_d < 0.0)) {
		throw std::logic_error("Time difference is negative.");
	}

	// Save the difference
	data[ref].setWallClockTime(WCLK_d);
	data[ref].setCPUTime(CPU_d);
}

void EPerf::addKernelDataVolumes(int KernelID, int DeviceID, long long inBytes,	long long outBytes) {

	// Check IDs
	checkKernelExistance(KernelID);
	checkDeviceExistance(DeviceID);

	// Add measurements
//	dvolume[std::pair<int, int>(KernelID, DeviceID)] = std::pair<long long, long long>(inBytes, outBytes);
	data[std::pair<int, int>(KernelID, DeviceID)].setDataVolumes(inBytes, outBytes);
}

std::ostream& operator<<(std::ostream &out, const EPerf &e) {

	std::map<int, std::string>::const_iterator it;

	out << "Devices:\n";
	for (it = e.devices.begin(); it != e.devices.end(); ++it) {
		out << "\tID: " << it->first << " Name: " << it->second << "\n";
	}
	out << "\n";

	out << "Kernels:\n";
	for (it = e.kernels.begin(); it != e.kernels.end(); ++it) {
		out << "\tID: " << it->first << " Name: " << it->second << "\n";
	}
	out << "\n";

	out << "Timings & Data volumes:\n";
	std::map<std::pair<int, int>, EPerfData>::const_iterator dit;
	for (dit = e.data.begin(); dit != e.data.end(); ++dit) {
		out << "K: " << dit->first.first << " D: " << dit->first.second << " " << dit->second << "\n";
	}
	
	return out;
}
}
