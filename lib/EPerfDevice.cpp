#include "../include/EPerf/EPerfDevice.h"

#include <cstring>

namespace ENHANCE {

EPerfDevice::EPerfDevice(std::string n) {
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

std::vector<char> EPerfDevice::convertToByteVector() const {

	std::vector<char> o;

	// Place the name of the device
	o.resize(name.size() + 1);
	memcpy(static_cast<void*>(&o[0]), static_cast<const char*>(name.c_str()), name.size() + 1);

	// Place all subdevices
	for (std::vector<int>::const_iterator it = subDevices.begin(); it != subDevices.end(); ++it) {
		// Increase size
		unsigned int pos = o.size();
		o.resize(o.size() + sizeof(int));
		memcpy(static_cast<void*>(&o[pos]), static_cast<const void*>(&(*it)),	sizeof(int));
	}

	return o;

}

}
