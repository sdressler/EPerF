#ifndef EPERF_CLOCK_H
#define EPERF_CLOCK_H

#include <ctime>
#include <iostream>
#include <vector>

#include "DB/IBDBObject.h"

namespace ENHANCE {

class EPerfClock : public IBDBObject {
private:
	struct timespec start;
	struct timespec stop;
	clockid_t clock;

public:
	EPerfClock(const clockid_t &c = CLOCK_REALTIME);

	inline void takeStartStamp() { clock_gettime(clock, &start); }
	inline void takeStopStamp() { clock_gettime(clock, &stop); }

	friend bool operator<(const EPerfClock &x, const EPerfClock &y);

	friend std::ostream& operator<<(std::ostream &out, const EPerfClock &c);
	
	double getTimeDifference() const;

	virtual std::vector<char> convertToByteVector() const;
};
}

#endif /* EPERF_CLOCK_H */
