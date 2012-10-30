#include "../include/EPerf/EPerfClock.h"

#include <cstring>
#include <sstream>

namespace ENHANCE {

#ifdef __MACH__
EPerfClock::EPerfClock() {
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &clock);
}
#else
EPerfClock::EPerfClock(const clockid_t &c) {
	clock = c;
}
#endif

bool operator<(const EPerfClock &x, const EPerfClock &y) {
	if (x.start.tv_sec < y.start.tv_sec || x.start.tv_nsec < y.start.tv_nsec) {
		return true;
	}
	return false;
}

std::ostream& operator<<(std::ostream &out, const EPerfClock &c) {
	out << " Start: " << c.start.tv_sec << " s " << c.start.tv_nsec << " ns";
	out << " / Stop: " << c.stop.tv_sec  << " s " << c.stop.tv_nsec  << " ns\n";

	return out;
}

double EPerfClock::getTimeDifference() const {

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
    
std::vector<int> EPerfClock::getIntegerPairs() const {

    std::vector<int> x;

    x.push_back(start.tv_sec); x.push_back(start.tv_nsec);
    x.push_back(stop.tv_sec); x.push_back(stop.tv_nsec);

    return x;

}

//tByteVectorMap EPerfClock::convertToByteVectorMap() const {
/*
    tByteVectorMap map;

    std::string key = std::string("start");
    std::vector<char> value;

    std::stringstream ss;
    ss << start.tv_sec << ":" << start.tv_nsec;

    value.resize(ss.str().size() + 1);
    memcpy(
        static_cast<void*>(&value[0]),
        static_cast<void*>(const_cast<char*>(ss.str().c_str())),
        ss.str().size() + 1
    );

    map.insert(std::make_pair(key, value));
    
    key = std::string("stop");
    ss.str() = std::string();
    ss << stop.tv_sec << ":" << stop.tv_nsec;

    value.clear();
    value.resize(ss.str().size() + 1);
    memcpy(
        static_cast<void*>(&value[0]),
        static_cast<void*>(const_cast<char*>(ss.str().c_str())),
        ss.str().size() + 1
    );

    map.insert(std::make_pair(key, value));

    return map;

}
*/
}
