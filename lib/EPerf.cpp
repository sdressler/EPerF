/**
 * Implementation of EPerf C++ Interface
 *
 * @author	Sebastian Dressler (ZIB)
 * @date	2012-05-24
 *
 * */

#include <fstream>

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
	std::pair<std::map<int, EPerfDevice>::iterator, bool> r = devices.insert(
		std::pair<int, EPerfDevice>(ID, EPerfDevice(dName))
	);

	// Check for overwrite attempt
	if (r.second == false) {
		throw std::runtime_error("Device already exists.");
	}
}

// Add a subdevice to another device
void EPerf::addSubDeviceToDevice(const int ID, const int sID) {
	// Check if both ID's are the same
	if (ID == sID) {
		throw std::invalid_argument("ID == sID!");
	};

	// Check if both devices exist
	checkDeviceExistance(ID);
	checkDeviceExistance(sID);

	// Add sID to ID
	devices[ID].addSubDevice(sID);
}

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
/*
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
	*/

	// Save the timestamps
	data[ref].setClockTime(std::string("wclk"), std::pair<double, double>(WCLK_start, WCLK_stop));
	data[ref].setClockTime(std::string("cpuclk"), std::pair<double, double>(CPU_start, CPU_stop));

}

void EPerf::addKernelDataVolumes(int KernelID, int DeviceID, int64_t inBytes,	int64_t outBytes) {

	// Check IDs
	checkKernelExistance(KernelID);
	checkDeviceExistance(DeviceID);

	// Add measurements
//	dvolume[std::pair<int, int>(KernelID, DeviceID)] = std::pair<int64_t, int64_t>(inBytes, outBytes);
	data[std::pair<int, int>(KernelID, DeviceID)].setDataVolumes(inBytes, outBytes);
}

void EPerf::exportToJSONFile(const std::string &path) {

	// Try to open the file
	std::ofstream f(path.c_str(), std::ios_base::trunc);

	// Check if it's open
	if (!f.is_open()) {
		throw std::runtime_error("Could not open file!");
	}

}

std::ostream& operator<<(std::ostream &out, const EPerf &e) {

	std::map<int, EPerfDevice>::const_iterator dit;
	out << "Devices:\n";
	for (dit = e.devices.begin(); dit != e.devices.end(); ++dit) {
		out << "\tID: " << dit->first << " Name: " << dit->second << "\n";
	}
	out << "\n";

	std::map<int, std::string>::const_iterator kit;
	out << "Kernels:\n";
	for (kit = e.kernels.begin(); kit != e.kernels.end(); ++kit) {
		out << "\tID: " << kit->first << " Name: " << kit->second << "\n";
	}
	out << "\n";

	out << "Timings & Data volumes:\n";
	std::map<std::pair<int, int>, EPerfData>::const_iterator vit;
	for (vit = e.data.begin(); vit != e.data.end(); ++vit) {
		out << "K: " << vit->first.first << " D: " << vit->first.second << " " << vit->second << "\n";
	}
	
	return out;
}
}
