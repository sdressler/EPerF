#ifndef EPERF_DATA_H
#define EPERF_DATA_H

#include <map>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <ctime>

#include <inttypes.h>

#include "EPerfClock.h"
#include "EPerfSQLite.h"
//#include "DB/IBDBObject.h"

namespace ENHANCE {

class EPerfData : public IEPerfSQLite {

    friend class EPerf;

private:
	EPerfClock timestamp;			        ///< Vector to save the timestamp
	std::vector<EPerfClock> clocks;         ///< Vector to save different timings
	std::map<int, std::string> clockNames;  ///< Map to associate clock IDs with names

	int64_t inBytes;	///< Bytes to be transferred host -> device
	int64_t outBytes;	///< Bytes to be transferred device -> host

	int KernelID;		///< Holds the reference to the used kernel
	int DeviceID;		///< Holds the reference to the used device
    int ThreadID;       ///< Holds the reference to the used thread
    //int PID;            ///< Holds the PID of the caller

	//std::string kConfigHash; ///< A reference to the kernel configuration hash

	bool timerIsRunning;

public:
	/**
	 * Default constructor inserting the available clocks
	 *
	 * */
	EPerfData();

	virtual ~EPerfData() { }

	inline int getKernelID() const { return KernelID; }
	inline int getDeviceID() const { return DeviceID; }
    inline int getThreadID() const { return ThreadID; }
//    inline int getPID() const { return PID; }
	inline EPerfClock& getTimeStamp() const { return const_cast<EPerfClock&>(timestamp); }

	inline void startAllTimers() {
		timestamp.takeStartStamp();
		clocks[0].takeStartStamp();
//		clocks[1].takeStartStamp();
		timerIsRunning = true;
	}

	inline void stopAllTimers() {
	    if (timerIsRunning == false) {
	        throw std::runtime_error("Timer was not started!");
	    }

//		clocks[1].takeStopStamp();
		clocks[0].takeStopStamp();
		timestamp.takeStopStamp();

		timerIsRunning = false;
	}


    inline friend bool operator<(const EPerfData &x, const EPerfData &y) {
        // Test whether we have different kernels
        if (x.KernelID < y.KernelID) {
            return true;
        } else {
            // Kernels are equal, check more properties
            // Test whether we are on different devices
            if (x.DeviceID < y.DeviceID) {
                return true;
            } else {
/*    			// Equal devices, configurations should be different
                if (x.kConfigHash < y.kConfigHash) {
                    return true;
                } else {
*/
                    // Not -> Timestamps are left
                    return x.timestamp < y.timestamp;
    //			}
            }
        }

        // Error otherwise
        return false;			
    }


	/**
	 * Print the class' content to a stream. If the stream is an ofstream the
	 * output is converted to JSON.
	 *
	 * @param[in,out] out The corresponding stream
	 * @param[in] d A reference to the data
	 * @retval std::ostream The returning stream
	 * */
	friend std::ostream& operator<<(std::ostream &out, const EPerfData &d);

    virtual std::vector<std::string> createSQLInsertObj() const;
//	virtual tByteVectorMap convertToByteVectorMap() const;

};
}

#endif /* EPERF_DATA_H */
