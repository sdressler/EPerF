#ifndef EPERF_DEVICE_H
#define EPERF_DEVICE_H

#include <string>
#include <vector>

namespace ENHANCE {
class EPerfDevice {
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

		if (&out == &std::cout || &out == &std::cerr) {
			out << d.name;
	
			if (d.subDevices.size() != 0) {
				out << " Subdevices: ";
				for (std::vector<int>::const_iterator it = d.subDevices.begin(); it != d.subDevices.end(); ++it) {
					out << *it << " ";
				}
			}
		} else {
			
			out << "\"name\": \"" << d.name << "\"";

			if (d.subDevices.size() != 0) {
				out << ",\n";
				out << "\"subdevices\": [\n";
				for (std::vector<int>::const_iterator it = d.subDevices.begin(); it != d.subDevices.end(); ++it) {
					out << "{\"id\": " << *it << "\n";
					
					++it;
					if (it == d.subDevices.end()) {
						out << "}\n";
					} else {
						out << "},\n";
					}
					--it;

				}
				out << "]\n";
			} else {
				out << "\n";
			}

		}

		return out;
	}

};
}

#endif /* EPERF_DEVICE_H */
