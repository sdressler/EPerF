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
#include "DB/IBDBObject.h"

namespace ENHANCE {

class EPerfData : public IBDBObject {
private:

	EPerfClock timestamp;			///< Vector to save the timestamp
	std::vector<EPerfClock> clocks; ///< Vector to save different timings
	std::map<int, std::string> clockNames; ///< Map to associate clock IDs with names

	int64_t inBytes;	///< Bytes to be transferred host -> device
	int64_t outBytes;	///< Bytes to be transferred device -> host

	int KernelID;		///< Holds the reference to the used kernel
	int DeviceID;		///< Holds the reference to the used device

	std::string kConfigHash; ///< A reference to the kernel configuration hash

public:
	/**
	 * Default constructor inserting the available clocks
	 *
	 * */
	EPerfData();

	inline int getKernelID() const { return KernelID; }
	inline int getDeviceID() const { return DeviceID; }
	inline EPerfClock& getTimeStamp() const { return const_cast<EPerfClock&>(timestamp); }

	inline void startAllTimers() {
		timestamp.takeStartStamp();
		clocks[0].takeStartStamp();
		clocks[1].takeStartStamp();
	}

	inline void stopAllTimers() {
		clocks[1].takeStopStamp();
		clocks[0].takeStopStamp();
		timestamp.takeStopStamp();
	}

	// Set reference to KernelID / DeviceID and kernel configuration hash
	void setKernelDeviceReference(const int k, const int d);
    void setKernelConfigReference(const int k, const std::string &kconf);

	friend bool operator<(const EPerfData &x, const EPerfData &y);
	
	/**
	 * Set the data volumes
	 *
	 * @param[in] i The volume to be transferred host -> device (in) in Bytes
	 * @param[in] o The volume to be transferred device -> host (out) in Bytes
	 * */
	void setDataVolumes(const int64_t i, const int64_t o);

	/**
	 * Print the class' content to a stream. If the stream is an ofstream the
	 * output is converted to JSON.
	 *
	 * @param[in,out] out The corresponding stream
	 * @param[in] d A reference to the data
	 * @retval std::ostream The returning stream
	 * */
	friend std::ostream& operator<<(std::ostream &out, const EPerfData &d);

	virtual std::vector<char> convertToByteVector() const;
	//virtual tBDBObjectMap convertToBDBObjectMap(std::string prefix);

};
}

#endif /* EPERF_DATA_H */
