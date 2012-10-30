#include "../include/EPerf/EPerfKernelConf.h"

namespace ENHANCE {

std::ostream& operator<<(std::ostream &out, const EPerfKernelConf &c) {

    out << "\n";
    for (tStringMap::const_iterator it = c.begin(); it != c.end(); ++it) {
       out << "\t\t" << it->first << " : " << it->second << "\n";
    }

    return out;
}

void EPerfKernelConf::insertKernelConfPair(std::string key, std::string value) {
    insert(std::pair<std::string, std::string>(key, value));
}

std::string EPerfKernelConf::getKernelConfHash() const {

    std::string s;

    for (std::map<std::string, std::string>::const_iterator it = begin(); it != end(); ++it) {
        s += it->first;
        s += it->second;
    }

    // Generate hash
    return sha256(s);
}
    
std::string EPerfKernelConf::sha256(const std::string &s) const {
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

std::vector<std::string> EPerfKernelConf::createSQLInsertObj() const {

    // Rewrite entire KConf
    
    std::vector<std::string> x;

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

    return x;

}
}
