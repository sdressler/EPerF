#include "../include/EPerf/EPerfData.h"
#include "../include/EPerf/EPerf.h"

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

//	for (unsigned int i = 0; i < d.clocks.size(); i++) {
//		std::string clockName = d.clockNames.find(i)->second;
//		out << "Clock " << clockName << "[s]:\t" << d.clocks[i].getTimeDifference() << "\n";
//	}

    out << "CPU Clock [s]:\t" << d.clocks[0].getTimeDifference() << "\n";

	out << "Data in[byte]: " << d.inBytes << ", Data out[byte]: " << d.outBytes << "\n";

	return out;
}

std::vector<std::string> EPerfData::createSQLInsertObj() const {

    std::vector<std::string> x;
    std::stringstream q;

    //q   << "INSERT OR IGNORE INTO data "
    q   << "INSERT INTO data "
        << "("
        <<      "id_kernel, id_device, "
        <<      "ts_start, "
        <<      "ts_stop, "
        <<      "cpuclock_start, "
        <<      "cpuclock_stop, "
        <<      "tid, data_in, data_out,"
        <<      "id_experiment"
        << ") "
        << "VALUES ("
        <<      KernelID << ", "
        <<      DeviceID << ", ";

    std::vector<double> clockval = timestamp.getDoublePairs(EPerf::experiment_starttime);
    q   << clockval[0] << ", " << clockval[1] << ", ";

    std::vector<EPerfClock>::const_iterator it;
    struct timespec dummy = {0,0};
    for (it = clocks.begin(); it != clocks.end(); ++it) {
        clockval = it->getDoublePairs(dummy);
        q   << clockval[0] << ", " << clockval[1] << ", ";
    }

      q <<      ThreadID << ", "
        <<      inBytes << ", "
        <<      outBytes << ", "
        <<      "'" << EPerf::getExperimentID() << "'"
        << ")";

    x.push_back(q.str());

    return x;

}

}

