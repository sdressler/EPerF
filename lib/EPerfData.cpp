#include "../include/EPerf/EPerfData.h"

#include <cstring>

namespace ENHANCE {
EPerfData::EPerfData() {
	KernelID = -1;
	DeviceID = -1;
	inBytes = 0;
	outBytes = 0;
	clocks.push_back(EPerfClock(CLOCK_MONOTONIC));
	clockNames[0] = "wall-clock";

	clocks.push_back(EPerfClock(CLOCK_PROCESS_CPUTIME_ID));
	clockNames[1] = "CPU clock";
}

void EPerfData::setKernelDeviceReference(const int k, const std::string &kconf, const int d) {
	// Check if they are already set
	if (KernelID > -1 || DeviceID > -1) {
		throw std::runtime_error("Kernel ID and Device ID references are already set!");
	}

	KernelID = k;
	kConfigHash = kconf;
	DeviceID = d;
}

bool operator<(const EPerfData &x, const EPerfData &y) {
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
/*			// Equal devices, configurations should be different
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

void EPerfData::setDataVolumes(const int64_t i, const int64_t o) {
	inBytes = i; outBytes = o;
}

std::ostream& operator<<(std::ostream &out, const EPerfData &d) {

	out << "KernelID: " << d.KernelID << " DeviceID: " << d.DeviceID << "\n";
    out << "Config Hash: " << d.kConfigHash << "\n";
	
	out << std::scientific << std::setprecision(9);
	out << "Timestamp: " << d.timestamp;

	for (unsigned int i = 0; i < d.clocks.size(); i++) {
		std::string clockName = d.clockNames.find(i)->second;
		out << "Clock " << clockName << "[s]:\t" << d.clocks[i].getTimeDifference() << "\n";
	}

	out << "Data in[byte]: " << d.inBytes << ", Data out[byte]: " << d.outBytes << "\n";

	return out;
}

std::vector<char> EPerfData::convertToByteVector() const {

	std::vector<char> o;
	
    unsigned int offset;

    offset += kConfigHash.size();

    // Save configuration hash
    o.resize(kConfigHash.size());
    memcpy(
        static_cast<void*>(&o[0]),
        static_cast<void*>(const_cast<char*>(kConfigHash.c_str())),
        offset
    );

	// Save {in,out}Bytes
	o.resize(2 * sizeof(int64_t));
	memcpy(static_cast<void*>(&o[offset]), static_cast<const void*>(&inBytes), sizeof(int64_t));
	memcpy(static_cast<void*>(&o[offset + 4]), static_cast<const void*>(&outBytes), sizeof(int64_t));

	// Save all clocks
	for (std::vector<EPerfClock>::const_iterator it = clocks.begin(); it != clocks.end(); ++it) {
		std::vector<char> bVec = it->convertToByteVector();
		// Append to end
		o.insert(o.end(), bVec.begin(), bVec.end());
	}

	return o;

}
}

