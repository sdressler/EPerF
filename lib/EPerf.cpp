/**
 * Implementation of EPerf C++ Interface
 *
 * @author  Sebastian Dressler (ZIB)
 * @date    2012-05-24
 *
 * */

#include <fstream>
#include <sstream>
#include <iostream>

#include <omp.h>

//#include <boost/uuid/uuid_generators.hpp>
//#include <boost/uuid/uuid_io.hpp>
#include <ctime>

#include "../include/EPerf/EPerf.h"
#include "../include/EPerf/EPerfSQLite.h"
#include "../include/EPerf/EPerfC.h"
#include "../include/EPerf/EPerfHelpers.h"

namespace ENHANCE {

uint64_t        EPerf::experiment_id        = 0;
std::string     EPerf::experiment_name      = "";
long int        EPerf::experiment_date      = 0;
struct timespec EPerf::experiment_starttime = {0,0};

EPerf::EPerf(const std::string &_dbFileName, const std::string &expName) {

    if (_dbFileName == std::string("")) {
        dbFileName = std::string("eperf.db");
    } else {
        dbFileName = _dbFileName;
    }

    DMSG("DB: " << _dbFileName);
    DMSG("ExpName: " << expName);

    max_units = MAX_UNITS; // omp_get_num_procs();
    id_relative = 0;

	DMSG("Max Units: " << max_units);

	// Create experiment UUID
/*
	std::stringstream id;
    boost::uuids::random_generator gen;
	boost::uuids::uuid u = gen();
	id << u;
*/
	EPerfSQLite db(dbFileName);

	experiment_id   = db.getLastExperimentID() + 1;
    experiment_name = expName;
    experiment_date = static_cast<long int>(std::time(NULL));

    clock_gettime(CLOCK_REALTIME, &experiment_starttime);

    DMSG("ExpID: " << experiment_id);

    // Write the experiment
    std::stringstream q;

    q << "INSERT OR IGNORE INTO experiments (id, date, name, start_s, start_ns) VALUES("
      << "'" << experiment_id   << "', "
             << experiment_date << ", "
      << "'" << experiment_name << "', "
             << experiment_starttime.tv_sec << ", "
             << experiment_starttime.tv_nsec << ")";

    db.executeInsertQuery(q.str());

    DMSG("Initialized");

}

EPerf::~EPerf() { DMSG("EPerF Destructor."); }

void EPerf::finalize() { commitToDB(); }

void EPerf::resizeTemporaryDataObject() {

    size_t num_threads = max_units; //omp_get_num_procs();

	size_t k_size = kernels.size();
	size_t d_size = devices.size();

	size_t requestedSize = (k_size + d_size) * num_threads;

	data.clear();
    data.resize(requestedSize, std::list<EPerfData>());

}

// Add a new kernel with unique ID and optional kernel name
tKernelMap::iterator EPerf::addKernel(const int ID, const std::string &kName) {
    // Try to insert, if it already exists -> exception
    std::pair<tKernelMap::iterator, bool> r = kernels.insert(
        std::pair<int, EPerfKernel>(ID, EPerfKernel(ID, kName))
    );

    // Check for overwrite attempt
    if (r.second == false) {
        throw std::runtime_error("Kernel already exists.");
    }

    resizeTemporaryDataObject();

    return r.first;
}

// Add a new device with unique ID and optional device name
tDeviceMap::iterator EPerf::addDevice(const int ID, const std::string &dName) {
    // Try to insert
    std::pair<tDeviceMap::iterator, bool> r = devices.insert(
        std::pair<int, EPerfDevice>(ID, EPerfDevice(ID, dName))
    );

    // Check for overwrite attempt
    if (r.second == false) {
        throw std::runtime_error("Device already exists.");
    }

    resizeTemporaryDataObject();

    return r.first;
}

// Add a subdevice to another device
void EPerf::addSubDeviceToDevice(const int ID, const int sID) {
    // Check if both ID's are the same
    if (ID == sID) {
        throw std::invalid_argument("ID == sID!");
    };

    // Check if both devices exist
    checkDeviceExistance(ID);
    checkDeviceExistance(sID);

    // Add sID to ID
    tDeviceMap::iterator it = devices.find(ID);
    (it->second).addSubDevice(sID);
}

// Take a TimeSpec structure and convert it to double, seconds
double EPerf::convTimeSpecToDoubleSeconds(const struct timespec &t) {
    double tt = (double)t.tv_nsec * 1.0e-9;
    tt += (double)t.tv_sec;

    return tt;
}

void EPerf::addKernelDataVolumes(int KernelID, int DeviceID, int64_t inBytes, int64_t outBytes) {
    
    t_eperf_id eperf_id = getEPerfID(
        (uint32_t)(getThreadID() - getThreadGroupID()),
        (uint16_t)KernelID,
        (uint16_t)DeviceID
    );

    uint64_t id = thr_map[eperf_id];
    
    DMSG("TMR KDV   K: " << getEPerfKernelID(eperf_id) <<
                  " D: " << getEPerfDeviceID(eperf_id) <<
                  " T: " << getEPerfThreadID(eperf_id) <<
                  " EID: " << std::hex << eperf_id << std::dec <<
                  " ID: " << id);

    data[id].back().inBytes = inBytes;
    data[id].back().outBytes = outBytes;

}

std::ostream& operator<<(std::ostream &out, const EPerf &e) {

    std::map<int, EPerfDevice>::const_iterator dit;
    out << "Devices:\n";
    for (dit = e.devices.begin(); dit != e.devices.end(); ++dit) {
        out << "\tID: " << dit->first << " Name: " << dit->second << "\n";
    }
    out << "\n";

    tKernelMap::const_iterator kit; 
    out << "Kernels:\n";
    for (kit = e.kernels.begin(); kit != e.kernels.end(); ++kit) {
        out << "\tID: " << kit->first << " Name: " << kit->second << "\n";
    }
    out << "\n";
    
    out << "Timings & Data volumes:\n";

    for (unsigned int i = 0; i < e.data.size(); i++) {
        
        std::list<EPerfData>::const_iterator it;
        for (it = e.data[i].begin(); it != e.data[i].end(); ++it) {
            out << *it << "\n";
        }

    }

    out << "\n";

    return out;
}
}
