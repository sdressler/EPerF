#include "../include/EPerf/EPerfExperiment.h"

#include <cstring>
#include <sstream>
#include <sys/utsname.h>

namespace ENHANCE {

EPerfExperiment::EPerfExperiment(const std::string &_name) {

    name = _name;

    // Get system informations
    struct utsname _osinfo;
    uname(&_osinfo);

    std::stringstream ss;
    ss << _osinfo.sysname << " " << _osinfo.release;
    osinfo = ss.str();

    machine = std::string(_osinfo.machine);

    // Get the timestamp
    start = time(NULL);
    stop  = time(NULL);

}

std::ostream& operator<<(std::ostream &out, const EPerfExperiment &e) {

    out << "Experiment: " << e.name << " on " << e.osinfo << ", " << e.machine << "\n";
    out << "Started: " << e.start << "Stopped: " << e.stop << "\n";

	return out;
}
}
