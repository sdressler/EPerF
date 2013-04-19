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
#include <semaphore.h>

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

#ifdef DEBUG
    #define \
        DMSG(m) \
        _Pragma("omp critical") \
        std::cerr << "EPerF: " << m << "\n";
#else
    #define DMSG(m)
#endif

#include <map>
#include <cmath>
#include <string>
#include <list>
#include <vector>
#include <iostream>
#include <iomanip>
#include <stdexcept>

#include <sys/syscall.h>

//#include <db_cxx.h>

#include "EPerfContainer.h"
	

namespace ENHANCE {

//typedef std::vector<std::vector<EPerfData> > tDataVector;
typedef std::vector<std::list<EPerfData> > tDataVector;

class EPerf : public EPerfContainer {
private:
/*
    class ID_type {
    private:
        int K;
        int D;
        int T;

    public:
        inline ID_type(int _K, int _D, int _T) : K(_K), D(_D), T(_T) { }

        // This implements strict weak ordering for the map
        inline bool operator<(const ID_type &oID) const {
            if (K != oID.K) {
                return (K < oID.K);
            }

            if (D != oID.D) {
                return (D < oID.D);
            }

            return (T < oID.T);
        }
    };
*/
//    typedef std::map<ID_type, EPerfData> tTempDataMap;
//    tTempDataMap tempData;

    //typedef std::vector<std::vector<EPerfData> > tDataVector;

    tDataVector data;
    std::vector<unsigned int> dataSizeVector;

    inline pid_t getThreadID() { return syscall(SYS_gettid); }

    std::string dbFileName;
    
    static std::string experiment_id;
    static std::string experiment_name;
    static long int    experiment_date;

	unsigned long max_threads;
	std::map<pid_t, uint64_t> thr_map;
	uint64_t tid_relative;

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
    inline void checkDeviceExistance(const int ID) {
        if (devices.find(ID) == devices.end()) {
            throw std::invalid_argument("Device ID not valid.");
        }
    }
	
	/**
	 * Checks whether a specific kernel ID exists
	 *
	 * @exception std::invalid_argument If the kernel does not exist
	 * @param[in] ID The ID of the kernel
	 * */
    inline void checkKernelExistance(const int ID) {
        if (kernels.find(ID) == kernels.end()) {
            throw std::invalid_argument("Kernel ID not valid.");
        }
    }

//	int BDB_getDevice(Db *sbdbp, const Dbt *pkey, const Dbt *pvalue, Dbt *skey);

	void resizeTemporaryDataObject();

public:

	static std::string& getExperimentID() { return experiment_id; }

    /**
     *
     * EPerF constructor with optional name for DB file. If the
     * name is empty, the framework selects "eperf.db" automatically.
     *
     * @param[in] _dbFileName Where to store the DB
     * */
	EPerf(const std::string &_dbFileName, const std::string &_expName);
	~EPerf();

	/**
	 *
	 * Commit the collected data to a Berkeley DB. If the DB does not yet exist
	 * it is created.
	 * 
	 * */
    void finalize();
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
	tDeviceMap::iterator addDevice(const int ID, const std::string &dName = std::string());

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
	void startTimer(const int KernelID, const int DeviceID);

	/**
	 * Stops the time measurement for a specific kernel / device combination
	 *
	 * @exception std::invalid_argument If the kernel ID or the device ID is invalid (what() provides details)
	 * @exception std::runtime_error If the starttime was not recorded
	 * @exception std::logic_error If the time difference is negative
	 * @param[in] KernelID The ID of the kernel
	 * @param[in] DeviceID The ID of the device
	 * */
	void stopTimer(const int KernelID, const int DeviceID);

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
