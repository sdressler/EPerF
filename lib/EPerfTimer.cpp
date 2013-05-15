/*
 * EPerfTimer.cpp
 *
 *  Created on: Apr 19, 2013
 *      Author: bzcdress
 */

#include <omp.h>
#include "../include/EPerf/EPerf.h"
#include "../include/EPerf/EPerfHelpers.h"

namespace ENHANCE {

// Start the time measurement of a specific kernel
void EPerf::startTimer(const int KernelID, const int DeviceID) {

    pid_t sys_tid = getThreadID();

    #pragma omp critical
    {
        if (thr_map.find(sys_tid) == thr_map.end()) {
            thr_map[sys_tid] = tid_relative;
            tid_relative++;
        }
    }

    uint64_t id = thr_map[sys_tid];

    if (id > max_threads) { throw std::runtime_error("Thread overflow."); }

    DMSG("TMR START K-ID: " << KernelID << " D-ID: " << DeviceID <<
                  " T-ID: " << id << " (SYS-T-ID: " << sys_tid << ")");

    data[id].push_back(EPerfData());
    data[id].back().startAllTimers();

};

// Stop the time measurement and save the measured time
void EPerf::stopTimer(const int KernelID, const int DeviceID) {

    pid_t sys_tid = getThreadID();
    uint64_t id = thr_map[sys_tid];

    DMSG("TMR STOP  K-ID: " << KernelID << " D-ID: " << DeviceID <<
                  " T-ID: " << id << " (SYS-T-ID: " << sys_tid << ")");

    EPerfData *edata = &(data[id].back());

    edata->stopAllTimers();
    edata->KernelID = KernelID;
    edata->DeviceID = DeviceID;
    edata->ThreadID = id;

}

}
