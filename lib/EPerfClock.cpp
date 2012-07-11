#include "../include/EPerf/EPerfClock.h"

#include <cstring>

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

std::vector<char> EPerfClock::convertToByteVector() const {

	std::vector<char> o;

	// Copy the clockid
	o.resize(sizeof(clockid_t));
	memcpy(static_cast<void*>(&o[0]), static_cast<const void*>(&clock), sizeof(clockid_t));

	// Copy the timespec structs
	int pos = o.size();
	o.resize(o.size() + 2 * sizeof(struct timespec));
	memcpy(static_cast<void*>(&o[pos]), static_cast<const void*>(&start), sizeof(struct timespec));

	pos += sizeof(struct timespec);
	memcpy(static_cast<void*>(&o[pos]), static_cast<const void*>(&stop), sizeof(struct timespec));

	return o;
}
}
