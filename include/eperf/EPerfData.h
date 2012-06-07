#ifndef EPERF_DATA_H
#define EPERF_DATA_H

#include <map>
#include <string>

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
}

#endif /* EPERF_DATA_H */
