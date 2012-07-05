#include "../include/EPerf/EPerfKernel.h"

#include <cstring>

namespace ENHANCE {
EPerfKernel::EPerfKernel(std::string s) {
    name = s;

    // Create "empty" hash
    EPerfKernelConf c;
    actConfHash = c.getKernelConfHash();
}

std::vector<char> EPerfKernel::convertToByteVector() const {

	std::vector<char> o;
	
	// Place the name of the kernel
	o.resize(name.size() + 1);
	memcpy(static_cast<void*>(&o[0]), static_cast<const char*>(name.c_str()), name.size() + 1);

	return o;

}

void EPerfKernel::insertKernelConf(const EPerfKernelConf &c) {

    std::string hash = c.getKernelConfHash();
    config.insert(std::pair<std::string, EPerfKernelConf>(hash, c));
    std::cout << "Inserted configuration: " << hash << "\n";

}
/*
void EPerfKernel::insertAndActivateKernelConf(const EPerfKernelConf &c) {
    
    std::string hash = c.getKernelConfHash();
    config.insert(std::pair<std::string, EPerfKernelConf>(hash, c));
    actConfHash = hash;

    std::cout << "Activated configuration: " << hash << "\n";

}
*/
void EPerfKernel::activateKernelConfWithPrototype(const EPerfKernelConf &proto) {
    
    activateKernelConfWithHash(proto.getKernelConfHash());

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
