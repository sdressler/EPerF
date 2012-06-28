#ifndef EPERF_KERNEL_H
#define EPERF_KERNEL_H

#include <iostream>
#include <string>
#include <map>
#include <sstream>

//#include <openssl/sha.h>

#include "EPerfKernelConfiguration.h"

#include "DB/IBDBObject.h"

namespace ENHANCE {

typedef std::map<std::string, EPerfKernelConfiguration> tKConfMap;

class EPerfKernel : public IBDBObject {
private:
	std::string name; ///< Holds the kernel name
	tKConfMap config; ///< Map for kernel configurations

	std::string actConfHash; ///< Identifier, which configuration is currently used

public:
	/**
	 * Create a new kernel with a name for easier identification
	 *
	 * @param[in] s The name of the new kernel
	 * */
	EPerfKernel(std::string s);

	const std::string& getActiveConfigurationHash() { return actConfHash; }

    void insertAndActivateKernelConfiguration(const EPerfKernelConfiguration &c);
    void activateKernelConfigurationWithPrototype(const EPerfKernelConfiguration &proto);
    void activateKernelConfigurationWithHash(const std::string &hash);
	
    friend std::ostream& operator<<(std::ostream &out, const EPerfKernel &k);

	virtual std::vector<char> convertToByteVector() const;
};
}

#endif /* EPERF_KERNEL_H */
