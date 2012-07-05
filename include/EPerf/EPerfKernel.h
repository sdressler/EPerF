#ifndef EPERF_KERNEL_H
#define EPERF_KERNEL_H

#include <iostream>
#include <string>
#include <map>
#include <sstream>

#include "EPerfKernelConf.h"

#include "DB/IBDBObject.h"

namespace ENHANCE {

typedef std::map<std::string, EPerfKernelConf> tKConfMap;

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

	const std::string& getActiveConfHash() { return actConfHash; }

    void insertKernelConf(const EPerfKernelConf &c);
    void activateKernelConfWithPrototype(const EPerfKernelConf &proto);
    void activateKernelConfWithHash(const std::string &hash);
	
    friend std::ostream& operator<<(std::ostream &out, const EPerfKernel &k);

	virtual std::vector<char> convertToByteVector() const;
};
}

#endif /* EPERF_KERNEL_H */
