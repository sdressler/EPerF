#include "../include/EPerf/EPerfData.h"

#include <cstring>

namespace ENHANCE {
EPerfData::EPerfData() {
	KernelID = -1;
	DeviceID = -1;
	inBytes = 0;
	outBytes = 0;
	
    #ifdef __MACH__
        clocks.push_back(EPerfClock());
    #else
        clocks.push_back(EPerfClock(CLOCK_MONOTONIC));
    #endif
    clockNames[0] = "wall-clock";

    #ifdef __MACH__
        clocks.push_back(EPerfClock());
    #else
	    clocks.push_back(EPerfClock(CLOCK_PROCESS_CPUTIME_ID));
    #endif
    clockNames[1] = "CPU clock";
}

bool operator<(const EPerfData &x, const EPerfData &y) {
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

std::ostream& operator<<(std::ostream &out, const EPerfData &d) {

	out << "KernelID: "  << d.KernelID
        << " DeviceID: " << d.DeviceID 
        << " PID: " << d.PID
        << " ThreadID: " << d.ThreadID
        << "\n";
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

tByteVectorMap EPerfData::convertToByteVectorMap() const {

    tByteVectorMap map;

    // Save the configuration hash
    std::string key = std::string("hash");
    std::vector<char> value;

    value.resize(kConfigHash.size() + 1);
    memcpy(
        static_cast<void*>(&value[0]),
        static_cast<const char*>(kConfigHash.c_str()),
        kConfigHash.size() + 1
    );
    map.insert(std::make_pair(key, value));

    // Save the in/out Bytes
    key = "bytein";
    value.clear();
    value.resize(sizeof(int));
    memcpy(
        static_cast<void*>(&value[0]),
        static_cast<const void*>(&inBytes),
        sizeof(int64_t)
    );
    map.insert(std::make_pair(key, value));
    
    key = "byteout";
    value.clear();
    value.resize(sizeof(int));
    memcpy(
        static_cast<void*>(&value[0]),
        static_cast<const void*>(&outBytes),
        sizeof(int64_t)
    );
    map.insert(std::make_pair(key, value));

    // Save all clocks
    for (unsigned int i = 0; i < clocks.size(); i++) {

        key = clockNames.find(i)->second;
        tByteVectorMap clock = clocks[i].convertToByteVectorMap();

        for (tByteVectorMap::const_iterator it = clock.begin(); it != clock.end(); ++it) {
            key += std::string(":") + it->first;
            map.insert(std::make_pair(key, it->second));
        }
    }

    return map;

}
}

