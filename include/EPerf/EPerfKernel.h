#ifndef EPERF_KERNEL_H
#define EPERF_KERNEL_H

#include <iostream>
#include <string>
#include <map>
#include <sstream>

#include "EPerfKernelConf.h"
#include "EPerfSQLite.h"

namespace ENHANCE {

typedef std::map<std::string, EPerfKernelConf> tKConfMap;

class EPerfKernel : public IEPerfSQLite {
private:
    int id;           ///< Holds the ID of the kernel
	std::string name; ///< Holds the kernel name
	tKConfMap config; ///< Map for kernel configurations

	std::string actConfHash; ///< Identifier, which configuration is currently used

public:
	/**
	 * Create a new kernel with a name for easier identification
	 *
     * @param[in] id The id of the new kernel
	 * @param[in] s The name of the new kernel
	 * */
	EPerfKernel(int id, std::string s);

	const std::string& getActiveConfHash() { return actConfHash; }

    void insertKernelConf(const EPerfKernelConf &c);
    void activateKernelConfWithPrototype(const EPerfKernelConf &proto);
    void activateKernelConfWithHash(const std::string &hash);

    // Return the iterators to the conf map
    std::pair<
        tKConfMap::iterator,
        tKConfMap::iterator
    > getConfMapIterators() {
     
        return std::make_pair(config.begin(), config.end());   

    }

    friend std::ostream& operator<<(std::ostream &out, const EPerfKernel &k);

    virtual std::vector<std::string> createSQLInsertObj() const;
//	virtual tByteVectorMap convertToByteVectorMap() const;
};
}

#endif /* EPERF_KERNEL_H */
