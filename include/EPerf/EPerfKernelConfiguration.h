#ifndef EPERF_KERNEL_CONFIGURATION_H
#define EPERF_KERNEL_CONFIGURATION_H

#include <inttypes.h>
#include <ctime>
#include <openssl/sha.h>

#include "EPerfData.h"

namespace ENHANCE {

typedef std::map<std::string, std::string> tStringMap;

class EPerfKernelConfiguration : private tStringMap {
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

    friend std::ostream& operator<<(std::ostream &out, const EPerfKernelConfiguration &c);

};
}

#endif /* EPERF_KERNEL_CONFIGURATION_H */
