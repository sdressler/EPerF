/** 
 * Header for ENHANCE Performance Framework (ePerF).
 *
 * @author	Sebastian Dressler (ZIB)
 * @date	2012-05-24
 *
 * \addtogroup ePerF
 * @{
 *
 * */

#ifndef EPERF_H
#define EPERF_H

// C header
#include <time.h>
#include <unistd.h>
#include <inttypes.h>

// This only checks whether we are on a POSIX system or not
#ifndef _POSIX_TIMERS
	#error _POSIX_TIMERS not available
#endif
#ifndef _POSIX_THREAD_CPUTIME
	#error _POSIX_THREAD_CPUTIME not available
#endif
#ifndef _POSIX_MONOTONIC_CLOCK
	#error _POSIX_MONOTONIC_CLOCK not available
#endif

#ifdef __cplusplus

#include <map>
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <stdexcept>

namespace ENHANCE {

class EPerfData {
private:
	std::map<std::string, std::pair<double, double> > clocks; ///< map for saving clocks

	int64_t inBytes;	///< Bytes to be transferred host -> device
	int64_t outBytes;	///< Bytes to be transferred device -> host

	/**
	 * Provides the time difference of a specific clock
	 *
	 * @exception runtime_error If the time difference is negative
	 * @param[in] clk Clock identifier string
	 * @retval double The time difference
	 * */
	double getTimeDifferenceForClock(const std::string &clk) const {
		// Try getting the clock
		std::map<std::string, std::pair<double, double> >::const_iterator it = clocks.find(clk);

		if (it == clocks.end()) {
			throw std::runtime_error("Clock not found!");
		}
		
		double diff = it->second.second - it->second.first;
		// Check the difference is negative (should not be the case) 
		if (diff < 0.0) {
			throw std::logic_error("Time difference is negative.");
		}

		return diff;
	}

public:
	/**
	 * Default constructor inserting the available clocks
	 *
	 * */
	EPerfData() {
		clocks["wclk"] = std::pair<double, double>();
		clocks["cpuclk"] = std::pair<double, double>();
	}

	/**
	 * Set the wall clock start time
	 *
	 * @param[in] clk The reference string for selecting the correct clock
	 * @param[in] t The timestamp to be set
	 * @param[in] first If true, set the start time, if false set the stop time
	 * */
	void setClockTime(const std::string clk, const std::pair<double, double> t) {
		// Get the clock
		std::map<std::string, std::pair<double, double> >::iterator it = clocks.find(clk);

		if (it == clocks.end()) {
			throw std::runtime_error("Clock not found!");
		}

		// Set the time
		it->second = t;
	}
	
	/**
	 * Set the data volumes
	 *
	 * @param[in] i The volume to be transferred host -> device (in) in Bytes
	 * @param[in] o The volume to be transferred device -> host (out) in Bytes
	 * */
	void setDataVolumes(int64_t i, int64_t o) { inBytes = i; outBytes = o; }

	/**
	 * Print the class' content to a stream
	 *
	 * @param[in,out] out The corresponding stream
	 * @param[in] d A reference to the data
	 * @retval std::ostream The returning stream
	 * */
	friend std::ostream& operator<<(std::ostream &out, const EPerfData &d) {
		out << std::scientific << std::setw(16) << std::setprecision(9);
		out << "CPU Time[s]: " << d.getTimeDifferenceForClock("cpuclk") << ", ";
		out	<< "Wall Clock Time[s]: " << d.getTimeDifferenceForClock("wclk") << ", ";
		out << std::fixed << std::setprecision(0);
		out << "Data in[byte]: " << d.inBytes << ", " << "Data out[byte]: " << d.outBytes;
		return out;
	}
};

class EPerfDevice {
private:
	std::string name;				///< Device name
	std::vector<int> subDevices;	///< Vector of subdevices

public:
	/**
	 *
	 * Default constructor. Optionally sets a device name
	 *
	 * @param[in] n The optional device name
	 * */
	EPerfDevice(std::string n = std::string()) : name(n) { }

	/**
	 *
	 * Add a new subdevice, identified by it's ID.
	 *
	 * @param[in] s The ID of the subdevice to be added.
	 * */
	void addSubDevice(int s) {
		subDevices.push_back(s);
	}

	/**
	 *
	 * Print the device name to a stream
	 *
	 * @param[in,out] out The corresponding stream
	 * @param[in] d A reference to the data
	 * @retval std::ostream The returning stream
	 * */
	friend std::ostream& operator<<(std::ostream &out, const EPerfDevice &d) {
		out << d.name;

		if (d.subDevices.size() != 0) {
			out << " Subdevices: ";
			for (std::vector<int>::const_iterator it = d.subDevices.begin(); it != d.subDevices.end(); ++it) {
				out << *it << " ";
			}
		}

		return out;
	}

};

class EPerf {
private:
	static const clockid_t CPU_clockid = CLOCK_PROCESS_CPUTIME_ID; ///< Holds the clockid for CPU_CLOCK
	static const clockid_t WCLK_clockid = CLOCK_MONOTONIC; ///< Holds the clockid for Wall Clock
	std::map<int, std::string> kernels; ///< Holds the kernels with ID and name
	std::map<int, EPerfDevice> devices; ///< Holds the devices with ID and name
	std::map<std::pair<int, int>, std::vector<struct timespec> > ttimes; ///< Temporary placeholder for timespecs
	std::map<std::pair<int, int>, EPerfData> data; ///< Holds the performance data
	
	/**
	 * Captures the current time and converts it to double
	 *
	 * @param[in] t Pointer to the timespec variable for saving the result
	 * @return The time seconds converted to double
	 * */
	double convTimeSpecToDoubleSeconds(const struct timespec &t);

	/**
	 * Checks whether a specific device ID exists
	 *
	 * @exception std::invalid_argument If the device does not exist
	 * @param[in] ID The ID of the device
	 * */
	void checkDeviceExistance(int ID);
	
	/**
	 * Checks whether a specific kernel ID exists
	 *
	 * @exception std::invalid_argument If the kernel does not exist
	 * @param[in] ID The ID of the kernel
	 * */
	void checkKernelExistance(int ID);

public:
	/**
	 * Adds a new kernel to the framework.
	 *
	 * @exception std::runtime_error If a kernel with the given ID already exists
	 * @param[in] ID The unique ID of the kernel
	 * @param[in] kName The (optional) name of the kernel
	 * */
	void addKernel(int ID, const std::string &kName = std::string());

	/**
	 * Adds a new device to the framework.
	 *
	 * @exception std::runtime_error If a device with the given ID already exists
	 * @param[in] ID The unique ID of the device
	 * @param[in] dName The (optional) name of the device
	 * */
	void addDevice(int ID, const std::string &dName = std::string());

	/**
	 * Adds a new subdevice to an existing device.
	 *
	 * @exception std::runtime_error If the subdevice to be added or the device
	 * where the subdevice shall be added does not exist
	 * @exception std::invalid_argument If both ID's are the same.
	 *
	 * @param[in] ID The unique ID of the device where the subdevice shall be added
	 * @param[in] sID The unique ID of the subdevice to be added to the device
	 * */
	void addSubDeviceToDevice(const int ID, const int sID);

	/**
	 * Starts the time measurement for a specific kernel / device combination
	 *
	 * @exception std::invalid_argument If the kernel ID or the device ID is invalid (what() provides details)
	 * @param[in] KernelID The ID of the kernel
	 * @param[in] DeviceID The ID of the device
	 * */
	void startTimer(int KernelID, int DeviceID);

	/**
	 * Stops the time measurement for a specific kernel / device combination
	 *
	 * @exception std::invalid_argument If the kernel ID or the device ID is invalid (what() provides details)
	 * @exception std::runtime_error If the starttime was not recorded
	 * @exception std::logic_error If the time difference is negative
	 * @param[in] KernelID The ID of the kernel
	 * @param[in] DeviceID The ID of the device
	 * */
	void stopTimer(int KernelID, int DeviceID);

	/**
	 * Registers the data to be transferred for a specific kernel / device combination
	 *
	 * @exception std::invalid_argument If the kernel ID or the device ID is invalid (what() provides details)
	 * @param[in] KernelID The ID of the kernel
	 * @param[in] DeviceID The ID of the device
	 * @param[in] inBytes  The number of Bytes to be transferred from <b>host to device</b>
	 * @param[in] outBytes The number of Bytes to be transferred from <b>device to host</b>
	 * */
	void addKernelDataVolumes(int KernelID,	int DeviceID, int64_t inBytes, int64_t outBytes);

	/**
	 * Prints the content of the object to given std::ostream.
	 *
	 * @param[in,out] out Stream for output
	 * @param[in] e The object to be printed
	 * @retval The output stream
	 * */
	friend std::ostream& operator<<(std::ostream &out, const EPerf &e);

};
}

#else
typedef struct EPerf EPerf;
typedef struct EPerf eperf;
#endif

#endif /* EPERF_H */

/** @} */
