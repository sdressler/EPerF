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

<<<<<<< HEAD:include/eperf/EPerf.h
=======
#include <db_cxx.h>

>>>>>>> db:include/EPerf/EPerf.h
#include "EPerfContainer.h"

namespace ENHANCE {
class EPerf : public EPerfContainer {
private:
<<<<<<< HEAD:include/eperf/EPerf.h
	static const clockid_t CPU_clockid = CLOCK_PROCESS_CPUTIME_ID; ///< Holds the clockid for CPU_CLOCK
	static const clockid_t WCLK_clockid = CLOCK_MONOTONIC; ///< Holds the clockid for Wall Clock

	std::map<std::pair<int, int>, std::vector<struct timespec> > ttimes; ///< Temporary placeholder for timespecs
	std::map<std::pair<int, int>, time_t> tTimeStamps; ///< Temporary placeholder for timestamps
=======

	typedef std::map<tKernelDeviceID, EPerfData> tTempDataMap;
	tTempDataMap tempData; ///< Temporary measurement data
>>>>>>> db:include/EPerf/EPerf.h

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

	int BDB_getDevice(Db *sbdbp, const Dbt *pkey, const Dbt *pvalue, Dbt *skey);

public:

	/**
	 *
	 * Commit the collected data to a Berkeley DB. If the DB does not yet exist
	 * it is created.
	 * 
	 * */
	void commitToDB();

	/**
	 * Adds a new kernel to the framework.
	 *
	 * @exception std::runtime_error If a kernel with the given ID already exists
	 * @param[in] ID The unique ID of the kernel
	 * @param[in] kName The (optional) name of the kernel
	 * */
	tKernelMap::iterator addKernel(const int ID, const std::string &kName = std::string());

	/**
	 * Adds a new device to the framework.
	 *
	 * @exception std::runtime_error If a device with the given ID already exists
	 * @param[in] ID The unique ID of the device
	 * @param[in] dName The (optional) name of the device
	 * */
	tDeviceMap::iterator addDevice(int ID, const std::string &dName = std::string());

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
	 * Exports all devices, kernels and measurements to a JSON file
	 *
	 * @param[in] path Path to the file
	 * */
	void exportToJSONFile(const std::string &path);

	/**
	 * Imports devices, kernels and measurements from a JSON file
	 *
	 * @param[in] path Path to the file
	 * */
	void importFromJSONFile(const std::string &path);

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
