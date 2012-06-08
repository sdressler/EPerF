#ifndef EPERF_KERNEL_H
#define EPERF_KERNEL_H

#include <iostream>
#include <string>
#include <map>
#include <sstream>

#include <openssl/sha.h>

namespace ENHANCE {

typedef std::map<std::string, std::string> tKConfMap;

class EPerfKernel : private tKConfMap {
private:
	std::string name; ///< Holds the kernel name
//	tKConfMap config; ///< Map for kernel configurations
	
	std::string hash; ///< Holds the generated hash

	void genConfigHash() {
		std::string s;

		for (tKConfMap::const_iterator it = begin(); it != end(); ++it) {
			s += it->first;
			s += it->second;
		}

		// Generate hash
		hash = sha256(s);
	}

	std::string sha256(const std::string &s) {
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

public:
	/**
	 * Create a new kernel with a name for easier identification
	 *
	 * @param[in] s The name of the new kernel
	 * */
	EPerfKernel(std::string s) {
		name = s;
	}

	void insertNewConfigKeyValuePair(const std::string &key, const std::string &value) {
		// Use the insert of the map
		this->insert(std::pair<std::string, std::string>(key, value));

		// Regenerate the hash
		genConfigHash();
	}
	
	friend std::ostream& operator<<(std::ostream &out, const EPerfKernel &k) {
		out << k.name << ", Configuration:\n";
		out << "\tHash: " << k.hash << "\n";
		for (tKConfMap::const_iterator it = k.begin(); it != k.end(); ++it) {
			out << "\tKey: " << it->first << " Value: " << it->second << "\n";
		}
		return out;
	}
};
}

#endif /* EPERF_KERNEL_H */
