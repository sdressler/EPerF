#ifndef EPERF_KERNEL_CONFIGURATION_H
#define EPERF_KERNEL_CONFIGURATION_H

#include <inttypes.h>
#include <ctime>
#include <openssl/sha.h>

#include "EPerfData.h"

namespace ENHANCE {

class EPerfKernelConfiguration : private std::map<std::string, std::string> {
private:
    std::string sha256(const std::string &s) const;
    
public:
    std::string getConfigHash() const;

    /**
     * Insert a new configuration k / v pair.
     *
     * @param[in] key The key, identifying the configuration pair
     * @param[in] value The value for the configuration pair
     * */
    void insertConfigPair(std::string key, std::string value);

};
}

#endif /* EPERF_KERNEL_CONFIGURATION_H */
