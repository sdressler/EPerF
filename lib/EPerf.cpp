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

#include "../include/EPerf/EPerf.h"
#include "../include/EPerf/EPerfSQLite.h"
#include "../include/EPerf/EPerfC.h"

namespace ENHANCE {

sem_t synchronize;

EPerf::EPerf(const std::string &_dbFileName) {
    sem_init(&synchronize, 1, 1);

    if (_dbFileName == std::string("")) {
        dbFileName = std::string("eperf.db");
    } else {
        dbFileName = _dbFileName;
    }

	max_threads = omp_get_num_procs();

}

EPerf::~EPerf() {
    sem_destroy(&synchronize);
}

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
        for (unsigned int j = 0; j < dataSizeVector[i]; j++) {
            db.executeInsertQuery(data[i][j].createSQLInsertObj());
        }
    }

    db.endTransaction();

}

void EPerf::resizeTemporaryDataObject() {

    size_t num_threads = omp_get_num_procs();

	size_t k_size = kernels.size();
	size_t d_size = devices.size();

	size_t requestedSize = (k_size + d_size) * num_threads;

	//data.clear();
	data.resize(requestedSize, std::vector<EPerfData>(1000));
	dataSizeVector.resize(requestedSize, 0);

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

// Start the time measurement of a specific kernel
void EPerf::startTimer(const int KernelID, const int DeviceID, const EPerfKernelConf &c) {

    uint64_t ID = (KernelID + 1) * (DeviceID + 1) - 1;
    uint64_t position = omp_get_thread_num() + (ID * omp_get_num_threads());

    data[position][dataSizeVector[position]].startAllTimers();
};

// Stop the time measurement and save the measured time
void EPerf::stopTimer(const int KernelID, const int DeviceID) {

    uint64_t ID = (KernelID + 1) * (DeviceID + 1) - 1;
    uint64_t position = omp_get_thread_num() + (ID * omp_get_num_threads());
    data[position][dataSizeVector[position]].stopAllTimers();

    data[position][dataSizeVector[position]].KernelID = KernelID;
    data[position][dataSizeVector[position]].DeviceID = DeviceID;
    data[position][dataSizeVector[position]].ThreadID = omp_get_thread_num();

    dataSizeVector[position]++;

    if (data[position].size() - dataSizeVector[position] < 100) {
        data[position].resize(data[position].size() + 5000);
    }

}

void EPerf::addKernelDataVolumes(int KernelID, int DeviceID, int64_t inBytes, int64_t outBytes) {

    uint64_t ID = (KernelID + 1) * (DeviceID + 1) - 1;
    uint64_t position = omp_get_thread_num() + (ID * omp_get_num_threads());
    data[position][dataSizeVector[position]].inBytes = inBytes;
    data[position][dataSizeVector[position]].outBytes = outBytes;

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
        for (unsigned int j = 0; j < e.dataSizeVector[i]; j++) {
            out << e.data[i][j] << "\n";
        }
    }

    out << "\n";

    return out;
}
}
