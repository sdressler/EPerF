#ifndef EPERF_CONTAINER_H
#define EPERF_CONTAINER_H

#include "EPerfDevice.h"
#include "EPerfData.h"

#include "../Serialization/IJSONSerializable.h"

#include <string>
#include <sstream>

namespace ENHANCE {
class EPerfContainer : public IJSONSerializable {
private:
	typedef std::map<int, std::string> tKernelMap;
	typedef std::map<int, EPerfDevice> tDeviceMap;
	typedef std::map<std::pair<int, int>, EPerfData> tDataMap;

protected:
	tKernelMap kernels;	///< Holds the kernels with ID and name
	tDeviceMap devices;	///< Holds the devices with ID and name
	tDataMap data;		///< Holds the performance data

public:
	virtual std::string serializeToJSONString() const {
		std::stringstream ss;

		ss << "{\n";
		ss << "\"devices\": [\n";
		std::map<int, EPerfDevice>::const_iterator dit;
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

		std::map<int, std::string>::const_iterator kit;
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
		
		ss << "\"measurements\" : [\n";

		std::map<std::pair<int, int>, EPerfData>::const_iterator mit;
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
		ss << "}\n";

		return ss.str();
	}

};
}

#endif /* EPERF_CONTAINER_H */
