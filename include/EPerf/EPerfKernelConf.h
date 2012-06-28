#ifndef EPERF_KERNEL_CONFIG_H
#define EPERF_KERNEL_CONFIG_H

#include <inttypes.h>
#include <time.h>
#include <openssl/sha.h>

#ifdef __cplusplus

#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <vector>

namespace ENHANCE {

typedef std::map<std::string, std::string> tStringMap;

class EPerfKernelConf : private tStringMap {
private:
    std::string sha256(const std::string &s) const;
    
public:
    std::string getKernelConfHash() const;

    /**
     * Insert a new configuration k / v pair.
     *
     * @param[in] key The key, identifying the configuration pair
     * @param[in] value The value for the configuration pair
     * */
    void insertKernelConfPair(std::string key, std::string value);

    friend std::ostream& operator<<(std::ostream &out, const EPerfKernelConf &c);

};
}

#else
typedef struct EPerfKernelConf EPerfKernelConf;
#endif

#endif /* EPERF_KERNEL_CONFIG_H */
