#ifndef EPERF_KERNEL_CONFIG_H
#define EPERF_KERNEL_CONFIG_H

#include <inttypes.h>
#include <time.h>
//#include <openssl/sha.h>

#ifdef __cplusplus

#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <vector>

#include "EPerfSQLite.h"

namespace ENHANCE {

//typedef std::map<std::string, std::string> tStrStrMap;

class EPerfKernelConf : public IEPerfSQLite {
private:
    typedef std::map<std::size_t, std::pair<std::string, std::string> > t_KVMap;
    t_KVMap kv_map;
    std::size_t conf_hash;


    //std::string sha256(const std::string &s) const;
    
public:
    //std::string getKernelConfHash() const;

    /**
     * Insert a new configuration k / v pair.
     *
     * @param[in] key The key, identifying the configuration pair
     * @param[in] value The value for the configuration pair
     * */
    void insertKernelConfPair(std::string key, std::string value);

    friend std::ostream& operator<<(std::ostream &out, const EPerfKernelConf &c) {

        out << "\n";
        for (t_KVMap::const_iterator it = c.kv_map.begin(); it != c.kv_map.end(); ++it) {
            out << "\t\t" << it->second.first << " : " << it->second.second << "\n";
        }

        return out;
    }

    virtual std::vector<std::string> createSQLInsertObj() const;

};
}

#else
typedef struct EPerfKernelConf EPerfKernelConf;
#endif

#endif /* EPERF_KERNEL_CONFIG_H */
