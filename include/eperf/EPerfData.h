#ifndef EPERF_DATA_H
#define EPERF_DATA_H

#include <map>
#include <string>
#include <sstream>
#include <ctime>

#include "../Serialization/IJSONSerializable.h"

namespace ENHANCE {

typedef std::map<std::string, std::pair<double, double> >::const_iterator const_time_it;

class EPerfData : public IJSONSerializable {
private:
	std::map<std::string, std::pair<double, double> > clocks; ///< map for saving clocks

	int64_t inBytes;	///< Bytes to be transferred host -> device
	int64_t outBytes;	///< Bytes to be transferred device -> host

	time_t timeStamp;

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

	uint64_t pack754(long double f, unsigned bits, unsigned expbits) const {
		
		long double fnorm;
		int shift;
		long long sign, exp, significand;
		unsigned significandbits = bits - expbits - 1; // -1 for sign bit

		if (f == 0.0) return 0; // get this special case out of the way

		// check sign and begin normalization
		if (f < 0) { sign = 1; fnorm = -f; }
		else { sign = 0; fnorm = f; }

		// get the normalized form of f and track the exponent
		shift = 0;
		while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
		while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
		fnorm = fnorm - 1.0;

		// calculate the binary form (non-float) of the significand data
		significand = fnorm * ((1LL<<significandbits) + 0.5f);

		// get the biased exponent
		exp = shift + ((1<<(expbits-1)) - 1); // shift + bias

		// return the final answer
		return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
	}


public:
	/**
	 * Default constructor inserting the available clocks
	 *
	 * */
	EPerfData() : inBytes(0), outBytes(0) {
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
	 * Set the timestamp
	 *
	 * @param[in] t The timestamp
	 * */
	void setTimeStamp(const time_t t) {
		timeStamp = t;
	}
	
	/**
	 * Set the data volumes
	 *
	 * @param[in] i The volume to be transferred host -> device (in) in Bytes
	 * @param[in] o The volume to be transferred device -> host (out) in Bytes
	 * */
	void setDataVolumes(int64_t i, int64_t o) { inBytes = i; outBytes = o; }

	/**
	 * Print the class' content to a stream. If the stream is an ofstream the
	 * output is converted to JSON.
	 *
	 * @param[in,out] out The corresponding stream
	 * @param[in] d A reference to the data
	 * @retval std::ostream The returning stream
	 * */
	friend std::ostream& operator<<(std::ostream &out, const EPerfData &d) {
		
		out << std::scientific << std::setw(16) << std::setprecision(9);
		out << "Timestamp: " << d.timeStamp << ", ";
		out << "CPU Time[s]: " << d.getTimeDifferenceForClock("cpuclk") << ", ";
		out	<< "Wall Clock Time[s]: " << d.getTimeDifferenceForClock("wclk") << ", ";
		out << std::fixed << std::setprecision(0);
		out << "Data in[byte]: " << d.inBytes << ", " << "Data out[byte]: " << d.outBytes;
		
		return out;
	}

	// Used for serialization
	virtual std::string serializeToJSONString() const {
		std::stringstream ss;
		
		// Print the timings
		ss << "\"timestamp\": " << timeStamp << ",\n";
		ss << "\"timings\": {\n";
		for (const_time_it it = clocks.begin(); it != clocks.end(); ++it) {
			ss << "\"" << it->first << "\": {\n";
			ss << "\"start\": " << pack754(it->second.first, 64, 11) << ",\n";
			ss << "\"stop\": " << pack754(it->second.second, 64, 11) << "\n";

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

		return ss.str();
	}

};
}

#endif /* EPERF_DATA_H */
