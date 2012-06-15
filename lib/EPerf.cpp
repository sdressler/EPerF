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
tKernelMap::iterator EPerf::addKernel(int ID, const std::string &kName) {
	// Try to insert, if it already exists -> exception
	std::pair<tKernelMap::iterator, bool> r = kernels.insert(
		std::pair<int, EPerfKernel>(ID, EPerfKernel(kName))
	);

	// Check for overwrite attempt
	if (r.second == false) {
		throw std::runtime_error("Kernel already exists.");
	}

	return r.first;
}

// Add a new device with unique ID and optional device name
tDeviceMap::iterator EPerf::addDevice(int ID, const std::string &dName) {
	// Try to insert
	std::pair<tDeviceMap::iterator, bool> r = devices.insert(
		std::pair<int, EPerfDevice>(ID, EPerfDevice(dName))
	);

	// Check for overwrite attempt
	if (r.second == false) {
		throw std::runtime_error("Device already exists.");
	}

	return r.first;
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

	// Insert a new temporary object and get a reference to it
	// Possibly an entry already exists from KDV
	std::pair<tTempDataMap::iterator, bool> x;

	x = tempData.insert(
		std::pair<tKernelDeviceID, EPerfData>(
			tKernelDeviceID(KernelID, DeviceID), EPerfData()
		)
	);

	// Test whether this entry already exists
/*	if (!x.second) {
		throw std::runtime_error("Temporary entry already exists.");
	}*/

	// Start the timers
	(x.first)->second.startAllTimers();
};

// Stop the time measurement and save the measured time
void EPerf::stopTimer(int KernelID, int DeviceID) {

	// Get the entry
	tTempDataMap::iterator x = tempData.find(tKernelDeviceID(KernelID, DeviceID));
	if (x == tempData.end()) {
		throw std::runtime_error("Timer was not started!");
	}

	// Stop the timers
	x->second.stopAllTimers();

	// Copy to set and remove temporary entry
	x->second.setKernelDeviceReference(
		KernelID,
		kernels.find(KernelID)->second.getActiveConfigurationHash(),
		DeviceID
	);
	
	std::pair<tDataSet::const_iterator, bool> test = data.insert(x->second);
	if (!test.second) {
		throw std::runtime_error("Something went wrong on data insertion.");
	}

	tempData.erase(tKernelDeviceID(KernelID, DeviceID));

}

void EPerf::addKernelDataVolumes(int KernelID, int DeviceID, int64_t inBytes,	int64_t outBytes) {

	// Check IDs
	checkKernelExistance(KernelID);
	checkDeviceExistance(DeviceID);
	
	// Insert a new temporary object and get a reference to it
	// Possibly an entry already exists from KDV
	std::pair<tTempDataMap::iterator, bool> x;

	x = tempData.insert(
		std::pair<tKernelDeviceID, EPerfData>(
			tKernelDeviceID(KernelID, DeviceID), EPerfData()
		)
	);

	(x.first)->second.setDataVolumes(inBytes, outBytes);

}

std::ostream& operator<<(std::ostream &out, const EPerf &e) {

	for (tDataSet::const_iterator it = e.data.begin(); it != e.data.end(); ++it) {
		out << *it << "\n";
	}

/*
	std::map<int, EPerfDevice>::const_iterator dit;
	out << "Devices:\n";
	for (dit = e.devices.begin(); dit != e.devices.end(); ++dit) {
		out << "\tID: " << dit->first << " Name: " << dit->second << "\n";
	}
	out << "\n";

	tKernelMap::const_iterator kit;	
	out << "Kernels:\n";
	for (kit = e.kernels.begin(); kit != e.kernels.end(); ++kit) {
		out << "\tID: " << kit->first << " Name: " << kit->second << "\n";
	}
	out << "\n";
*/
	/**
	 * \todo{Change implementation to fit new internal layout of data
	 *
	out << "Timings & Data volumes:\n";
	std::map<std::pair<int, int>, EPerfData>::const_iterator vit;
	for (vit = e.data.begin(); vit != e.data.end(); ++vit) {
		out << "K: " << vit->first.first << " D: " << vit->first.second << " " << vit->second << "\n";
	}
	*/
	return out;
}
}
