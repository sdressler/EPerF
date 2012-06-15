#ifndef EPERF_CONTAINER_H
#define EPERF_CONTAINER_H

#include "EPerfKernel.h"
#include "EPerfDevice.h"

#include "../Serialization/IJSONSerializable.h"

#include <string>
#include <set>
#include <map>
#include <sstream>

namespace ENHANCE {
	
typedef std::map<int, EPerfKernel> tKernelMap;
typedef std::map<int, EPerfDevice> tDeviceMap;
typedef std::pair<int, int> tKernelDeviceID;
typedef std::set<EPerfData> tDataSet;

class EPerfContainer : public IJSONSerializable {

protected:
	tKernelMap kernels;	///< Holds the kernels with ID and name
	tDeviceMap devices;	///< Holds the devices with ID and name
	tDataSet data;		///< Holds the performance data

public:
	virtual std::string serializeToJSONString() const {
		std::stringstream ss;
/*
		ss << "{\n";
		ss << "\"devices\": [\n";
		tDeviceMap::const_iterator dit;
		for (dit = devices.begin(); dit != devices.end(); ++dit) {
			
			ss << "{" << dit->second.serializeToJSONString();

			++dit;
			if (dit == devices.end()) {
				ss << "}\n";
			} else {
				ss << "},\n";
			}
			--dit;
		}
		ss << "],\n";
		
		ss << "\"kernels\" : [\n";

		tKernelMap::const_iterator kit;
		for (kit = kernels.begin(); kit != kernels.end(); ++kit) {
			ss << "{\"ID\" : " << kit->first << ", \"name\" : \"" << kit->second << "\"}";
			
			// Last?
			kit++;
			if (kit != kernels.end()) {
				ss << ",\n";
			} else {
				ss << "\n";
		}
			kit--;
		}
		ss << "],\n";
*/
/*		
		ss << "\"measurements\" : [\n";

		tDataMap::const_iterator mit;
		for (mit = data.begin(); mit != data.end(); ++mit) {
			ss << "{\n";

			ss << "\"kernel\": " << mit->first.first << ",\n";
			ss << "\"device\": " << mit->first.second << ",\n";
		
			ss << mit->second.serializeToJSONString();

			// Last?
			mit++;
			if (mit != data.end()) {
				ss << "},\n";
			} else {
				ss << "}\n";
			}
			mit--;
		}
		ss << "]\n";
*/
//		ss << "}\n";

		return ss.str();
	}

};
}

#endif /* EPERF_CONTAINER_H */
