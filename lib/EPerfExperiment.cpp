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

}

std::ostream& operator<<(std::ostream &out, const EPerfExperiment &e) {

    out << "Experiment: " << e.name << " on " << e.osinfo << ", " << e.machine << "\n";
    out << "Started: " << e.start << "Stopped: " << e.stop << "\n";

	return out;
}

/*
tByteVectorMap EPerfExperiment::convertToByteVectorMap() const {

    tByteVectorMap map;

    std::string key = std::string("name");
    std::vector<char> value(name.size() + 1);
    memcpy(
        static_cast<void*>(&value[0]),
        static_cast<const char*>(name.c_str()),
        name.size() + 1
    );
    map.insert(make_pair(key, value));

    // OS info string
    key = "osinfo";
    value.clear();
    value.resize(osinfo.size() + 1);
    memcpy(
        static_cast<void*>(&value[0]),
        static_cast<const char*>(osinfo.c_str()),
        osinfo.size() + 1
    );
    map.insert(make_pair(key, value));

    // Machine info string
    key = "arch";
    value.clear();
    value.resize(machine.size() + 1);
    memcpy(
        static_cast<void*>(&value[0]),
        static_cast<const char*>(machine.c_str()),
        osinfo.size() + 1
    );
    map.insert(make_pair(key, value));

    // Start time
    key = "start";
    value.clear();
    std::stringstream ss;
    ss << start;
    value.resize(ss.str().size() + 1);
    memcpy(
        static_cast<void*>(&value[0]),
        static_cast<const char*>(ss.str().c_str()),
        ss.str().size() + 1
    );
    map.insert(make_pair(key, value));

    ss.str() = std::string();

    // Stop time
    key = "stop";
    value.clear();
    ss << stop;
    value.resize(ss.str().size() + 1);
    memcpy(
        static_cast<void*>(&value[0]),
        static_cast<const char*>(ss.str().c_str()),
        ss.str().size() + 1
    );
    map.insert(make_pair(key, value));
	
    return map;

}
*/
}
