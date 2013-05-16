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

    t_eperf_id eperf_id = getEPerfID(
        (uint32_t)(getThreadID() - getThreadGroupID()),
        (uint16_t)KernelID,
        (uint16_t)DeviceID
    );

    // Retrieve a relative ID
    #pragma omp critical
    {
        if (thr_map.find(eperf_id) == thr_map.end()) {
            thr_map[eperf_id] = id_relative;
            id_relative++;
        }
    }

    uint64_t id = thr_map[eperf_id];

    if (id > max_units) { throw std::runtime_error("Thread overflow."); }

    DMSG("TMR START K: " << getEPerfKernelID(eperf_id) <<
                  " D: " << getEPerfDeviceID(eperf_id) <<
                  " T: " << getEPerfThreadID(eperf_id) <<
                  " EID: " << std::hex << eperf_id << std::dec <<
                  " ID: " << id);

    data[id].push_back(EPerfData());
    data[id].back().startAllTimers();

};

// Stop the time measurement and save the measured time
void EPerf::stopTimer(const int KernelID, const int DeviceID) {
    
    t_eperf_id eperf_id = getEPerfID(
        (uint32_t)(getThreadID() - getThreadGroupID()),
        (uint16_t)KernelID,
        (uint16_t)DeviceID
    );

    uint64_t id = thr_map[eperf_id];
    
    DMSG("TMR STOP  K: " << getEPerfKernelID(eperf_id) <<
                  " D: " << getEPerfDeviceID(eperf_id) <<
                  " T: " << getEPerfThreadID(eperf_id) <<
                  " EID: " << std::hex << eperf_id << std::dec <<
                  " ID: " << id);

    EPerfData *edata = &(data[id].back());

    edata->stopAllTimers();
    edata->KernelID = getEPerfKernelID(eperf_id);
    edata->DeviceID = getEPerfDeviceID(eperf_id);
    edata->ThreadID = getEPerfThreadID(eperf_id);

}

}
