#ifndef EPERF_CONTAINER_H
#define EPERF_CONTAINER_H

#include "EPerfKernel.h"
#include "EPerfDevice.h"
#include "EPerfData.h"

#include "DB/IBDBObject.h"

#include <string>
#include <set>
#include <map>
#include <sstream>

namespace ENHANCE {
	
typedef std::map<int, EPerfKernel> tKernelMap;
typedef std::map<int, EPerfDevice> tDeviceMap;

//typedef std::set<EPerfData> tDataSet;

class EPerfContainer {

protected:
	tKernelMap kernels;	///< Holds the kernels with ID and name
	tDeviceMap devices;	///< Holds the devices with ID and name
	//tDataSet data;		///< Holds the performance data

	tDeviceMap& getDevices() { return devices; }

public:
//	virtual tBDBObjectMap convertToBDBObjectMap(std::string prefix);

};
}

#endif /* EPERF_CONTAINER_H */
