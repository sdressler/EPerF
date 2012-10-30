#include "../include/EPerf/EPerfDevice.h"

#include <cstring>

namespace ENHANCE {

EPerfDevice::EPerfDevice(int _id, std::string n) {
    id = _id;
	name = n;
}
	
void EPerfDevice::addSubDevice(int s) {
	subDevices.push_back(s);
}

std::ostream& operator<<(std::ostream &out, const EPerfDevice &d) {
	out << d.name;

	if (d.subDevices.size() != 0) {
		out << " Subdevices: ";
		std::vector<int>::const_iterator it;
		for (it = d.subDevices.begin(); it != d.subDevices.end(); ++it) {
			out << *it << " ";
		}
	}

	return out;
}

std::vector<std::string> EPerfDevice::createSQLInsertObj() const {

    std::vector<std::string> x;

    std::stringstream q;
    
    q   << "INSERT OR IGNORE INTO devices (id, name) VALUES("
        << id << ", '" << name << "')";

    x.push_back(q.str());

    std::vector<int>::const_iterator it;
    for (it = subDevices.begin(); it != subDevices.end(); ++it) {

        q.str("");
        q.clear();

        q << "INSERT OR IGNORE INTO subdevices (id_left, id_right) VALUES("
          << id << ", " << *it << ")";

        x.push_back(q.str());

    }

    return x;

}

/*
tByteVectorMap EPerfDevice::convertToByteVectorMap() const {

    tByteVectorMap map;

	// Place the name of the device
    std::string key = "name";
    std::vector<char> value(name.size() + 1);

    memcpy(
        static_cast<void*>(&value[0]),
        static_cast<const char*>(name.c_str()),
        name.size() + 1
    );

    map.insert(std::make_pair(key, value));

	// Place all subdevices
	for (std::vector<int>::const_iterator it = subDevices.begin(); it != subDevices.end(); ++it) {

        value.clear();
        value.resize(sizeof(int));

        memcpy(
            static_cast<void*>(&value[0]),
            static_cast<const void*>(&(*it)),
            sizeof(int)
        );

        std::stringstream ss;
        ss << std::distance(it, subDevices.begin());
        map.insert(std::make_pair(ss.str(), value));
	}

	return map;

}
*/
}
