#include "../include/EPerf/EPerfData.h"

#include <cstring>

namespace ENHANCE {
EPerfData::EPerfData() {
	KernelID = -1;
	DeviceID = -1;
	inBytes = 0;
	outBytes = 0;
	timerIsRunning = false;
/*
    #ifdef __MACH__
        clocks.push_back(EPerfClock());
    #else
        clocks.push_back(EPerfClock(CLOCK_MONOTONIC));
    #endif
    clockNames[0] = "wall-clock";
*/

    #ifdef __MACH__
        clocks.push_back(EPerfClock());
    #else
	    clocks.push_back(EPerfClock(CLOCK_PROCESS_CPUTIME_ID));
    #endif
    clockNames[0] = "CPU clock";
}

std::ostream& operator<<(std::ostream &out, const EPerfData &d) {

	out << "KernelID: "  << d.KernelID
        << " DeviceID: " << d.DeviceID 
//        << " PID: " << d.PID
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

std::vector<std::string> EPerfData::createSQLInsertObj() const {

    std::vector<std::string> x;
    std::stringstream q;

    q   << "INSERT OR IGNORE INTO data "
        << "("
        <<      "id_kernel, id_device, conf_hash, "
        <<      "ts_start_s, "
        <<      "ts_start_ns, "
        <<      "ts_stop_s, "
        <<      "ts_stop_ns, "
//        <<      "wclock_start_s, "
//        <<      "wclock_start_ns, "
//        <<      "wclock_stop_s, "
//        <<      "wclock_stop_ns, "
        <<      "cpuclock_start_s, "
        <<      "cpuclock_start_ns, "
        <<      "cpuclock_stop_s, "
        <<      "cpuclock_stop_ns, "
        <<      "tid, data_in, data_out"
        << ") "
        << "VALUES ("
        <<      KernelID << ", "
        <<      DeviceID << ", "
        <<      "'" << kConfigHash << "', ";

    std::vector<int> clockval = timestamp.getIntegerPairs();
    q   << clockval[0] << ", " << clockval[1] << ", " << clockval[2] << ", " << clockval[3] << ", ";
    
    std::vector<EPerfClock>::const_iterator it;
    for (it = clocks.begin(); it != clocks.end(); ++it) {
        clockval = it->getIntegerPairs();
        q << clockval[0] << ", " << clockval[1] << ", " << clockval[2] << ", " << clockval[3] << ", ";
    }

//    q   <<      PID << ", "
      q <<      ThreadID << ", "
        <<      inBytes << ", "
        <<      outBytes
        << ")";

    x.push_back(q.str());

    return x;

}

/*
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
*/
}

