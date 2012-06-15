#ifndef EPERF_DATA_H
#define EPERF_DATA_H

#include <map>
#include <string>
#include <sstream>
#include <ctime>

#include "../Serialization/IJSONSerializable.h"

namespace ENHANCE {

class EPerfClock {
private:
	struct timespec start;
	struct timespec stop;
	clockid_t clock;

public:
	EPerfClock(const clockid_t &c = CLOCK_REALTIME) {
		clock = c;
	}

	inline void takeStartStamp() { clock_gettime(clock, &start); }
	inline void takeStopStamp() { clock_gettime(clock, &stop); }

	friend bool operator<(const EPerfClock &x, const EPerfClock &y) {
		if (x.start.tv_sec < y.start.tv_sec || x.start.tv_nsec < y.start.tv_nsec) {
			return true;
		}
		return false;
	}

	friend std::ostream& operator<<(std::ostream &out, const EPerfClock &c) {
		out << " Start: " << c.start.tv_sec << " s " << c.start.tv_nsec << " ns";
		out << " / Stop: " << c.stop.tv_sec  << " s " << c.stop.tv_nsec  << " ns\n";

		return out;
	}
	
	double getTimeDifference() const {

		struct timespec temp;
		if ((stop.tv_nsec-start.tv_nsec)<0) {
			temp.tv_sec = stop.tv_sec-start.tv_sec-1;
			temp.tv_nsec = 1000000000+stop.tv_nsec-start.tv_nsec;
		} else {
			temp.tv_sec = stop.tv_sec-start.tv_sec;
			temp.tv_nsec = stop.tv_nsec-start.tv_nsec;
		}

		// Convert the difference to double
		double diff = (double)temp.tv_sec * 1.0e9 + (double)temp.tv_nsec;

		return diff / 1.0e9;
	}
};

class EPerfData : public IJSONSerializable {
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
	EPerfData() {
		KernelID = -1;
		DeviceID = -1;
		inBytes = 0;
		outBytes = 0;
		clocks.push_back(EPerfClock(CLOCK_MONOTONIC));
		clockNames[0] = "wall-clock";

		clocks.push_back(EPerfClock(CLOCK_PROCESS_CPUTIME_ID));
		clockNames[1] = "CPU clock";
	}

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
	void setKernelDeviceReference(const int k, const std::string &kconf, const int d) {
		// Check if they are already set
		if (KernelID > -1 || DeviceID > -1) {
			throw std::runtime_error("Kernel ID and Device ID references are already set!");
		}

		KernelID = k;
		kConfigHash = kconf;
		DeviceID = d;
	}

	friend bool operator<(const EPerfData &x, const EPerfData &y) {
		/*
		// First check if the IDs are different
		if (x.KernelID < y.KernelID || x.DeviceID < y.DeviceID) {
			return true;
		} else {
			// If they are equal, timestamps should be different
			return (x.timestamp < y.timestamp);
		}
*/

		// Test whether we have different kernels
		if (x.KernelID < y.KernelID) {
			return true;
		} else {
			// Kernels are equal, check more properties
			// Test whether we are on different devices
			if (x.DeviceID < y.DeviceID) {
				return true;
			} else {
				// Equal devices, configurations should be different
				if (x.kConfigHash < y.kConfigHash) {
					return true;
				} else {
					// Not -> Timestamps are left
					return x.timestamp < y.timestamp;
				}
			}
		}

		// Error otherwise
		return false;			
	}
	
	/**
	 * Set the data volumes
	 *
	 * @param[in] i The volume to be transferred host -> device (in) in Bytes
	 * @param[in] o The volume to be transferred device -> host (out) in Bytes
	 * */
	void setDataVolumes(const int64_t i, const int64_t o) { inBytes = i; outBytes = o; }

	/**
	 * Print the class' content to a stream. If the stream is an ofstream the
	 * output is converted to JSON.
	 *
	 * @param[in,out] out The corresponding stream
	 * @param[in] d A reference to the data
	 * @retval std::ostream The returning stream
	 * */
	friend std::ostream& operator<<(std::ostream &out, const EPerfData &d) {

		out << "KernelID: " << d.KernelID << " DeviceID: " << d.DeviceID << "\n";
	//	out << "Kernel configuration hash: " << d.kConfigHash << "\n";
		
		out << std::scientific << std::setprecision(9);
		out << "Timestamp: " << d.timestamp;

		for (unsigned int i = 0; i < d.clocks.size(); i++) {
			std::string clockName = d.clockNames.find(i)->second;
			out << "Clock " << clockName << "[s]:\t" << d.clocks[i].getTimeDifference() << "\n";
		}

		out << "Data in[byte]: " << d.inBytes << ", Data out[byte]: " << d.outBytes << "\n";

		return out;
	}

	// Used for serialization
	virtual std::string serializeToJSONString() const {
		std::stringstream ss;
/*		
		// Print the timings
		ss << "\"timestamp\": " << timeStamp << ",\n";
		ss << "\"timings\": {\n";
		for (const_time_it it = clocks.begin(); it != clocks.end(); ++it) {
			ss << "\"" << it->first << "\": {\n";
	//		ss << "\"start\": " << pack754(it->second.first, 64, 11) << ",\n";
	//		ss << "\"stop\": " << pack754(it->second.second, 64, 11) << "\n";

			// Last one?
			++it;
			if (it ==clocks.end()) {
				ss << "}\n";
			} else {
				ss << "},\n";
			}
			--it;
		}
		ss << "},\n";

		// Print the bytes
		ss << "\"in\": " << inBytes << ",\n";
		ss << "\"out\": " << outBytes << "\n";
*/
		return ss.str();
	}

};
}

#endif /* EPERF_DATA_H */
