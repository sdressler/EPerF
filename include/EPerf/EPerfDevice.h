#ifndef EPERF_DEVICE_H
#define EPERF_DEVICE_H

#include <string>
#include <vector>
#include <sstream>

#include "DB/IBDBObject.h"

namespace ENHANCE {
class EPerfDevice : public IBDBObject {
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
	EPerfDevice(std::string n = std::string());

	/**
	 *
	 * Add a new subdevice, identified by it's ID.
	 *
	 * @param[in] s The ID of the subdevice to be added.
	 * */
	void addSubDevice(int s);

	/**
	 *
	 * Print the device name to a stream. If the stream is a file the
	 * output is converted to JSON.
	 *
	 * @param[in,out] out The corresponding stream
	 * @param[in] d A reference to the data
	 * @retval std::ostream The returning stream
	 * */
	friend std::ostream& operator<<(std::ostream &out, const EPerfDevice &d);

	virtual std::vector<char> convertToByteVector() const;
//	virtual tBDBObjectMap convertToBDBObjectMap(std::string prefix);
};
}

#endif /* EPERF_DEVICE_H */
