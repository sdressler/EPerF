#include "../include/EPerf/EPerfContainer.h"

#include <algorithm>

namespace ENHANCE {
/*
std::map<std::string, Dbt> EPerfContainer::convertToBDBObjectMap(std::string prefix) {
	std::map<std::string, Dbt> o;

	// Convert all devices to BDB objects
	for (tDeviceMap::iterator it = devices.begin(); it != devices.end(); ++it) {
		std::stringstream ss;
		// Convert device ID to string
		ss << it->first;

		// Create a reference entry
		o.insert(std::pair<std::string, Dbt>("device:" + ss.str(), Dbt(NULL, 0)));

		// Retrieve the device object with ID as prefix
		tBDBObjectMap d = it->second.convertToBDBObjectMap(ss.str());

		// Insert the remaining data
		o.insert(d.begin(), d.end());
	}
	
	return o;

}
*/
}
