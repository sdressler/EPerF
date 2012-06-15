#ifndef EPERF_DEVICE_H
#define EPERF_DEVICE_H

#include <string>
#include <vector>
#include <sstream>

#include "../Serialization/IJSONSerializable.h"

namespace ENHANCE {
class EPerfDevice : public IJSONSerializable {
private:
	std::string name;				///< Device name
	std::vector<int> subDevices;	///< Vector of subdevices

public:
	/**
	 *
	 * Default constructor. Optionally sets a device name
	 *
	 * @param[in] n The optional device name
	 * */
	EPerfDevice(std::string n = std::string()) : name(n) { }

	/**
	 *
	 * Add a new subdevice, identified by it's ID.
	 *
	 * @param[in] s The ID of the subdevice to be added.
	 * */
	void addSubDevice(int s) {
		subDevices.push_back(s);
	}

	/**
	 *
	 * Print the device name to a stream. If the stream is a file the
	 * output is converted to JSON.
	 *
	 * @param[in,out] out The corresponding stream
	 * @param[in] d A reference to the data
	 * @retval std::ostream The returning stream
	 * */
	friend std::ostream& operator<<(std::ostream &out, const EPerfDevice &d) {
		out << d.name;
	
		if (d.subDevices.size() != 0) {
			out << " Subdevices: ";
			for (std::vector<int>::const_iterator it = d.subDevices.begin(); it != d.subDevices.end(); ++it) {
				out << *it << " ";
			}
		}

		return out;
	}

	// Used for serialization
	virtual std::string serializeToJSONString() const {
		std::stringstream ss;
		
		ss << "\"name\": \"" << name << "\"";

		if (subDevices.size() != 0) {
			ss << ",\n";
			ss << "\"subdevices\": [\n";
			for (std::vector<int>::const_iterator it = subDevices.begin(); it != subDevices.end(); ++it) {
				ss << "{\"id\": " << *it << "\n";
				
				++it;
				if (it == subDevices.end()) {
					ss << "}\n";
				} else {
					ss << "},\n";
				}
				--it;

			}
			ss << "]\n";
		} else {
			ss << "\n";
		}

		return ss.str();
	}

};
}

#endif /* EPERF_DEVICE_H */
