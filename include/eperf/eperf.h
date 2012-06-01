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
#include <string>
#include <vector>
#include <iostream>

namespace ENHANCE {

class EPerfData {
private:
	double wclock;		///< Wall Clock time
	double cputime;		///< CPU time
	long long inBytes;	///< Bytes to be transferred host -> device
	long long outBytes;	///< Bytes to be transferred device -> host

public:
	/**
	 * Set the Wall Clock Time
	 *
	 * @param[in] w The wall clock time to be set
	 * */
	void setWallClockTime(double w) { wclock = w; }

	/**
	 * Set the CPU Time
	 *
	 * @param[in] c The CPU Time to be set
	 * */
	void setCPUTime(double c) { cputime = c; }

	/**
	 * Set the data volumes
	 *
	 * @param[in] i The volume to be transferred host -> device (in) in Bytes
	 * @param[in] o The volume to be transferred device -> host (out) in Bytes
	 * */
	void setDataVolumes(long long i, long long o) { inBytes = i; outBytes = o; }

	/**
	 * Print the class' content to a stream
	 *
	 * @param[in,out] out The corresponding stream
	 * @param[in] d A reference to the data
	 * @retval std::ostream The returning stream
	 * */
	friend std::ostream& operator<<(std::ostream &out, const EPerfData &d) {
		out << "CPU Time: " << d.cputime << "s, "
			<< "Wall Clock Time: " << d.wclock << "s, "
			<< "Data in: " << d.inBytes << " Byte, "
			<< "Data out: " << d.outBytes;
		return out;
	}
};

class EPerf {
private:
	static const clockid_t CPU_clockid = CLOCK_PROCESS_CPUTIME_ID; ///< Holds the clockid for CPU_CLOCK
	static const clockid_t WCLK_clockid = CLOCK_MONOTONIC; ///< Holds the clockid for Wall Clock
	std::map<int, std::string> kernels; ///< Holds the kernels with ID and name
	std::map<int, std::string> devices; ///< Holds the devices with ID and name
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
	void addKernelDataVolumes(int KernelID,	int DeviceID, long long inBytes, long long outBytes);

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
