#include "../include/EPerf/EPerfData.h"

#include <cstring>

namespace ENHANCE {
EPerfData::EPerfData() {
	KernelID = -1;
	DeviceID = -1;
	ThreadID = 0;
	inBytes = 0;
	outBytes = 0;
	timerIsRunning = false;

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
        << " ThreadID: " << d.ThreadID
        << "\n";
	
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
        <<      "id_kernel, id_device, "
        <<      "ts_start_s, "
        <<      "ts_start_ns, "
        <<      "ts_stop_s, "
        <<      "ts_stop_ns, "
        <<      "cpuclock_start_s, "
        <<      "cpuclock_start_ns, "
        <<      "cpuclock_stop_s, "
        <<      "cpuclock_stop_ns, "
        <<      "tid, data_in, data_out"
        << ") "
        << "VALUES ("
        <<      KernelID << ", "
        <<      DeviceID << ", ";

    std::vector<int> clockval = timestamp.getIntegerPairs();
    q   << clockval[0] << ", " << clockval[1] << ", " << clockval[2] << ", " << clockval[3] << ", ";
    
    std::vector<EPerfClock>::const_iterator it;
    for (it = clocks.begin(); it != clocks.end(); ++it) {
        clockval = it->getIntegerPairs();
        q << clockval[0] << ", " << clockval[1] << ", " << clockval[2] << ", " << clockval[3] << ", ";
    }

      q <<      ThreadID << ", "
        <<      inBytes << ", "
        <<      outBytes
        << ")";

    x.push_back(q.str());

    return x;

}

}

