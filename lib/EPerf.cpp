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
/*
        // Write the configuration maps for the kernel
        std::pair<tKConfMap::iterator, tKConfMap::iterator> cMap = (it->second).getConfMapIterators();

        tKConfMap::iterator cit;
        for (cit = cMap.first; cit != cMap.second; ++cit) {
            db.executeInsertQuery(
                (cit->second).createSQLInsertObj()
            );
        }
*/
    }

    db.beginTransaction();

    for (tDataSet::iterator it = data.begin(); it != data.end(); ++it) {
        db.executeInsertQuery(it->createSQLInsertObj());
    }

    db.endTransaction();

}

void EPerf::resizeTemporaryDataObject() {

    #pragma omp parallel
    {
        if (omp_get_thread_num() == 0) {

//            size_t currentSize = tempData.size();

            size_t k_size = kernels.size();
            size_t d_size = devices.size();
            size_t num_threads = omp_get_num_threads();

            size_t requestedSize = (k_size + d_size) * num_threads;

            tempData.clear();

            for (size_t i = 0; i < requestedSize; i++) {
                tempData.push_back(EPerfData());
            }

        }
    }

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

//    checkKernelExistance(KernelID);
//    checkDeviceExistance(DeviceID);

    //uint64_t position = ;

    /* Lock operation */
    //sem_wait(&synchronize);

//    uint64_t position = omp_get_thread_num();
    uint64_t ID = (KernelID + 1) * (DeviceID + 1) - 1;
    uint64_t position = omp_get_thread_num() + (ID * omp_get_num_threads());
    tempData[position].startAllTimers();

    /* Unlock */
    //sem_post(&synchronize);

    //tempData[(KernelID + DeviceID) * omp_get_thread_num()].startAllTimers();

//    std::cerr << "Start Timer: " << getThreadID() << "\n";
    
/*
    #pragma omp critical
    {
        // Add the configuration to the kernel
//        kernels.find(KernelID)->second.insertKernelConf(c);
//        tempData[position].kConfigHash = c.getKernelConfHash();
    }
*/
    // Insert a new temporary object and get a reference to it
    // Possibly an entry already exists from KDV
    //std::pair<tTempDataMap::iterator, bool> *x = new std::pair<tTempDataMap::iterator, bool>();
    
/*
    tempData.insert(std::make_pair(
        ID_type(KernelID, DeviceID, getThreadID()), EPerfData()
    ));
*/

    //std::cout << "Thread: " << omp_get_thread_num() << " of "
    //        << omp_get_num_threads() << "\n";


    // Set the configuration reference
    //(x.first)->second.kConfigHash = c.getKernelConfHash();

    // Start the timers
    //(x->first)->second.startAllTimers();
};

// Stop the time measurement and save the measured time
void EPerf::stopTimer(const int KernelID, const int DeviceID) {

    //uint64_t position = ;

    uint64_t ID = (KernelID + 1) * (DeviceID + 1) - 1;
    uint64_t position = omp_get_thread_num() + (ID * omp_get_num_threads());
    tempData[position].stopAllTimers();

    /* Unlock */
    //sem_post(&synchronize);

    tempData[position].KernelID = KernelID;
    tempData[position].DeviceID = DeviceID;
    tempData[position].ThreadID = omp_get_thread_num();
    //tempData[position].PID = getpid();

    /*
//    std::cerr << "Stop Timer: " << getThreadID() << "\n";
   
    tTempDataMap::iterator x = tempData.find(ID_type(KernelID, DeviceID, getThreadID()));
   
    if (x == tempData.end()) {
        throw std::runtime_error("Timer was not started!");
    }

    // Stop the timers
    x->second.stopAllTimers();
//    (*x->second).stopAllTimers();
    
    // Copy to set and remove temporary entry
    x->second.KernelID = KernelID;
    x->second.DeviceID = DeviceID;
    x->second.ThreadID = getThreadID();
    x->second.PID = getpid();
*/

//    std::pair<tDataSet::const_iterator, bool> test =

    /* Lock operation */
    //sem_wait(&synchronize);

    #pragma omp critical
    {
        data.insert(tempData[position]);
    }
/*
    if (!test.second) {
        throw std::runtime_error("Something went wrong on data insertion.");
    }
*/
    /* Unlock operation */
    //sem_post(&synchronize);

    //tempData.erase(ID_type(KernelID, DeviceID, getThreadID()));

}

void EPerf::addKernelDataVolumes(int KernelID, int DeviceID, int64_t inBytes, int64_t outBytes) {

    // Check IDs
//    checkKernelExistance(KernelID);
//    checkDeviceExistance(DeviceID);

    /* LOCK */
//    sem_wait(&synchronize);
/*
    // Insert a new temporary object and get a reference to it
    // Possibly an entry already exists from KDV
    std::pair<tTempDataMap::iterator, bool> x;
    
    // Get or create a (new) entry 
    x = tempData.insert(std::make_pair(ID_type(KernelID, DeviceID, getThreadID()), EPerfData()));

    (x.first)->second.inBytes  = inBytes;
    (x.first)->second.outBytes = outBytes;

*/

    uint64_t ID = (KernelID + 1) * (DeviceID + 1) - 1;
    uint64_t position = omp_get_thread_num() + (ID * omp_get_num_threads());
    tempData[position].inBytes = inBytes;
    tempData[position].outBytes = outBytes;

    /* UNLOCK */
//    sem_post(&synchronize);

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
    for (tDataSet::const_iterator it = e.data.begin(); it != e.data.end(); ++it) {
        out << *it << "\n";
    }
    out << "\n";

    return out;
}
}
