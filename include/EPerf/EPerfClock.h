#ifndef EPERF_CLOCK_H
#define EPERF_CLOCK_H

#include <iostream>
#include <vector>

#include "DB/IBDBObject.h"

#ifdef __MACH__
    #include <mach/clock.h>
    #include <mach/mach.h>
#else
    #include <ctime>
#endif

namespace ENHANCE {

class EPerfClock : public IBDBObject {
private:

    // The following is for Mac OS X
    #ifdef __MACH__
        typedef clock_serv_t clockid_t;
        mach_timespec_t start;
        mach_timespec_t stop;
    #else
        struct timespec start;
	    struct timespec stop;
    #endif

	clockid_t clock;

public:
    #ifdef __MACH__
        EPerfClock();
    #else
	    EPerfClock(const clockid_t &c = CLOCK_REALTIME);
    #endif

	inline void takeStartStamp() {
        #ifdef __MACH__
            clock_get_time(clock, &start);
        #else
            clock_gettime(clock, &start);
        #endif
    }

	inline void takeStopStamp() {
        #ifdef __MACH__
            clock_get_time(clock, &stop);
        #else
            clock_gettime(clock, &stop);
        #endif
    }

	friend bool operator<(const EPerfClock &x, const EPerfClock &y);

	friend std::ostream& operator<<(std::ostream &out, const EPerfClock &c);
	
	double getTimeDifference() const;
	virtual tByteVectorMap convertToByteVectorMap() const;
};
}

#endif /* EPERF_CLOCK_H */
