/**
 * Implementation of EPerf C++ Interface
 *
 * @author  Sebastian Dressler (ZIB)
 * @date    2012-05-24
 *
 * */

#include <fstream>
#include <sstream>

#include <omp.h>

#include <boost/uuid/uuid_io.hpp>
#include <ctime>

#include "../include/EPerf/EPerf.h"
#include "../include/EPerf/EPerfSQLite.h"
#include "../include/EPerf/EPerfC.h"

namespace ENHANCE {

std::string EPerf::experiment_id   = "";
std::string EPerf::experiment_name = "";
long int    EPerf::experiment_date = 0;

EPerf::EPerf(const std::string &_dbFileName, const std::string &expName) {

    if (_dbFileName == std::string("")) {
        dbFileName = std::string("eperf.db");
    } else {
        dbFileName = _dbFileName;
    }

	max_threads = omp_get_num_procs();

	// Create experiment UUID
	std::stringstream id;
	boost::uuids::uuid u;
	id << u;

	experiment_id = id.str();
    experiment_name = expName;
    experiment_date = static_cast<long int>(std::time(NULL));

}

EPerf::~EPerf() { }

void EPerf::finalize() { commitToDB(); }

void EPerf::commitToDB() {

    EPerfSQLite db(dbFileName);

    for (tDeviceMap::iterator it = devices.begin(); it != devices.end(); ++it) {

            db.executeInsertQuery(
                (it->second).createSQLInsertObj()
            );

    }

    for (tKernelMap::iterator it = kernels.begin(); it != kernels.end(); ++it) {

            db.executeInsertQuery(
                (it->second).createSQLInsertObj()
            );
    }

    db.beginTransaction();

    for (unsigned int i = 0; i < data.size(); i++) {

        //tDataVector::const_iterator it;
        std::list<EPerfData>::const_iterator it;
        for (it = data[i].begin(); it != data[i].end(); ++it) {
            db.executeInsertQuery(it->createSQLInsertObj());
        }

    }

    db.endTransaction();

    // Write the experiment
    std::stringstream q;

    q << "INSERT OR IGNORE INTO experiments (id, date, name) VALUES("
      << "'" << experiment_id   << "', "
             << experiment_date << ", "
      << "'" << experiment_name << "')";

    db.executeInsertQuery(q.str());

/*
    q   << "INSERT OR IGNORE INTO kernels (id, name) VALUES("
        << id << ", '" << name << "')";
*/

}

void EPerf::resizeTemporaryDataObject() {

    size_t num_threads = omp_get_num_procs();

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

// TODO: KernelConfiguration
// Start the time measurement of a specific kernel
void EPerf::startTimer(const int KernelID, const int DeviceID) {

    uint64_t ID = (KernelID + 1) * (DeviceID + 1) - 1;
    uint64_t position = omp_get_thread_num() + (ID * omp_get_num_threads());

    data[position].push_back(EPerfData());
    data[position].back().startAllTimers();

};

// Stop the time measurement and save the measured time
void EPerf::stopTimer(const int KernelID, const int DeviceID) {

    uint64_t ID = (KernelID + 1) * (DeviceID + 1) - 1;
    uint64_t position = omp_get_thread_num() + (ID * omp_get_num_threads());

    EPerfData *edata = &(data[position].back());
/*
    data[position].back().stopAllTimers();
    data[position].back().KernelID = KernelID;
    data[position].back().DeviceID = DeviceID;
    data[position].back().ThreadID = omp_get_thread_num();
*/

    edata->stopAllTimers();
    edata->KernelID = KernelID;
    edata->DeviceID = DeviceID;
    edata->ThreadID = omp_get_thread_num();    

}

void EPerf::addKernelDataVolumes(int KernelID, int DeviceID, int64_t inBytes, int64_t outBytes) {

    uint64_t ID = (KernelID + 1) * (DeviceID + 1) - 1;
    uint64_t position = omp_get_thread_num() + (ID * omp_get_num_threads());

    data[position].back().inBytes = inBytes;
    data[position].back().outBytes = outBytes;

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
