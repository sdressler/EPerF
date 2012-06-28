#include "../include/EPerf/EPerfKernelConfiguration.h"

namespace ENHANCE {

void EPerfKernelConfiguration::insertConfigPair(std::string key, std::string value) {
    insert(std::pair<std::string, std::string>(key, value));
}

std::string EPerfKernelConfiguration::getConfigHash() const {

    std::string s;

    for (std::map<std::string, std::string>::const_iterator it = begin(); it != end(); ++it) {
        s += it->first;
        s += it->second;
    }

    // Generate hash
    return sha256(s);
}
    
std::string EPerfKernelConfiguration::sha256(const std::string &s) const {
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, s.c_str(), s.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}
}
