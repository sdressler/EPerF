/*
 * EPerfTimer.cpp
 *
 *  Created on: Apr 19, 2013
 *      Author: bzcdress
 */

#include <omp.h>
#include "../include/EPerf/EPerf.h"

namespace ENHANCE {

// Start the time measurement of a specific kernel
void EPerf::startTimer(const int KernelID, const int DeviceID) {

    uint64_t ID = (KernelID + 1) * (DeviceID + 1) - 1;
    pid_t sys_tid = getThreadID();

    #pragma omp critical
    {
        if (thr_map.find(sys_tid) == thr_map.end()) {
            thr_map[sys_tid] = tid_relative;
            tid_relative++;
        }
    }

    uint64_t tid = thr_map[sys_tid];

    if (tid > max_threads) { throw std::runtime_error("Thread overflow."); }

    uint64_t position = tid + (ID * max_threads);

    DMSG("TMR START K" << KernelID << " D" << DeviceID <<
                  " T" << tid << " (" << sys_tid << ")"
                  " I" << ID << " P" << position);

    data[position].push_back(EPerfData());
    data[position].back().startAllTimers();

};

// Stop the time measurement and save the measured time
void EPerf::stopTimer(const int KernelID, const int DeviceID) {

    uint64_t ID = (KernelID + 1) * (DeviceID + 1) - 1;
    uint64_t tid = thr_map[getThreadID()];
    uint64_t position = tid + (ID * max_threads);

    DMSG("TMR STOP  K" << KernelID << " D" << DeviceID <<
                      " T" << tid << " (" << getThreadID() << ")"
                      " I" << ID << " P" << position);

    EPerfData *edata = &(data[position].back());

    edata->stopAllTimers();
    edata->KernelID = KernelID;
    edata->DeviceID = DeviceID;
    edata->ThreadID = tid;

}

}
