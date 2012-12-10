#include "../include/EPerf/EPerfKernel.h"

#include <cstring>

namespace ENHANCE {
EPerfKernel::EPerfKernel(int _id, std::string s) {
    id = _id;
    name = s;

    // Create "empty" hash
    EPerfKernelConf c;
    //actConfHash = c.igetKernelConfHash();
}

std::vector<std::string> EPerfKernel::createSQLInsertObj() const {

    std::vector<std::string> x;

    std::stringstream q;
    
    q   << "INSERT OR IGNORE INTO kernels (id, name) VALUES("
        << id << ", '" << name << "')";

    x.push_back(q.str());

    return x;

}

void EPerfKernel::insertKernelConf(const EPerfKernelConf &c) {

//    std::string hash = c.getKernelConfHash();
    //config.insert(std::pair<std::string, EPerfKernelConf>(hash, c));
//    std::cout << "Inserted configuration: " << hash << "\n";

}

void EPerfKernel::activateKernelConfWithPrototype(const EPerfKernelConf &proto) {
    
    //activateKernelConfWithHash(proto.getKernelConfHash());

}

void EPerfKernel::activateKernelConfWithHash(const std::string &hash) {
    
    tKConfMap::const_iterator it = config.find(hash);
    if (it == config.end()) {
        throw std::runtime_error("Conf not found!");
    }

    actConfHash = hash;

}

std::ostream& operator<<(std::ostream &out, const EPerfKernel &k) {
    out << k.name << ", Conf(s):\n";

    for (tKConfMap::const_iterator it = k.config.begin(); it != k.config.end(); ++it) {
        out << "\tHash: " << it->first << "\n";
        out << "\tKeys & Values: " << it->second << "\n";
    }
    
    return out;
}

}
