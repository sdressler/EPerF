#include "../include/EPerf/EPerfKernel.h"

#include <cstring>

namespace ENHANCE {
EPerfKernel::EPerfKernel(std::string s) {
    name = s;

    // Create "empty" hash
    EPerfKernelConfiguration c;
    actConfHash = c.getConfigHash();
}

std::vector<char> EPerfKernel::convertToByteVector() const {

	std::vector<char> o;
	
	// Place the name of the kernel
	o.resize(name.size() + 1);
	memcpy(static_cast<void*>(&o[0]), static_cast<const char*>(name.c_str()), name.size() + 1);

	return o;

}
   
void EPerfKernel::insertAndActivateKernelConfiguration(const EPerfKernelConfiguration &c) {
    
    std::string hash = c.getConfigHash();
    config.insert(std::pair<std::string, EPerfKernelConfiguration>(hash, c));
    actConfHash = hash;

    std::cout << "Activated configuration: " << hash << "\n";

}

void EPerfKernel::activateKernelConfigurationWithPrototype(const EPerfKernelConfiguration &proto) {
    
    activateKernelConfigurationWithHash(proto.getConfigHash());

}

void EPerfKernel::activateKernelConfigurationWithHash(const std::string &hash) {
    
    tKConfMap::const_iterator it = config.find(hash);
    if (it == config.end()) {
        throw std::runtime_error("Configuration not found!");
    }

    actConfHash = hash;

}

}
