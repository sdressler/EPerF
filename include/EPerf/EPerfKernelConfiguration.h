#ifndef EPERF_KERNEL_CONFIGURATION_H
#define EPERF_KERNEL_CONFIGURATION_H

#include <inttypes.h>
#include <ctime>

#include "EPerfData.h"

namespace ENHANCE {

//typedef std::map<time_t, EPerfData> tDataMap;
typedef std::map<std::string, std::string> tConfMap;

class EPerfKernelConfiguration {
private:
//	uint64_t inBytes;
//	uint64_t outBytes;

//	tDataMap measurements;
	tConfMap config;

/*	std::string hash; ///< Holds the generated hash

	void hashConfigs() {
		std::string s;

		for (std::map<std::string, std::string>::const_iterator it = begin(); it != end(); ++it) {
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
*/
public:
	/**
	 * Insert a new configuration k / v pair.
	 *
	 * @param[in] key The key, identifying the configuration pair
	 * @param[in] value The value for the configuration pair
	 * */
/*	void insertConfigPair(std::string key, std::string value) {
		configs[key] = value;
	}
*/
	/**
	 * Returns a value of a configuration identified by the given key
	 *
	 * @param[in] key The key, used for identification
	 * @retval std::string The corresponding value
	 * */
/*	std::string getConfigValueByKey(std::string key) {
		return configs[key];
	}

	friend std::ostream& operator<<(std::ostream &out, const EPerfKernel &k) {
		out << k.name;
		return out;
	}
*/
};
}

#endif /* EPERF_KERNEL_CONFIGURATION_H */
