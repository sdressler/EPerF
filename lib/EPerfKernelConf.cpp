#include "../include/EPerf/EPerfKernelConf.h"

#include <stdexcept>
//#include <boost/functional/hash.hpp>

namespace ENHANCE {

void EPerfKernelConf::insertKernelConfPair(std::string key, std::string value) {

    throw std::runtime_error("Kernel conf used, but not available!");

    // Create a hash
/*    
    boost::hash<std::string> hasher;
    std::size_t hash = hasher(key + value);

    // Insert
    kv_map.insert((std::make_pair(hash, std::make_pair(key, value))));

    // Compute overall hash and set
    std::stringstream ss;
    for (t_KVMap::iterator it = kv_map.begin(); it != kv_map.end(); ++it) {
        ss << key << value;
    }
    conf_hash = hasher(ss.str());
*/
}

std::vector<std::string> EPerfKernelConf::createSQLInsertObj() const {

    // Rewrite entire KConf
    std::vector<std::string> x;
/*
    std::stringstream q;
    
    tStringMap::const_iterator it;
    for (it = begin(); it != end(); ++it) {

        q.str("");
        q.clear();

        q   << "INSERT OR IGNORE INTO kernelconfigurations (hash, key, value) "
            << "VALUES ("
            <<      "'" << getKernelConfHash() << "',"
            <<      "'" << it->first << "',"
            <<      "'" << it->second << "'"
            << ")";

        x.push_back(q.str());

    } 
*/
    return x;

}
}
