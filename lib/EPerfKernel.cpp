#include "../include/EPerf/EPerfKernel.h"

#include <cstring>

namespace ENHANCE {
std::vector<char> EPerfKernel::convertToByteVector() const {

	std::vector<char> o;
	
	// Place the name of the kernel
	o.resize(name.size() + 1);
	memcpy(static_cast<void*>(&o[0]), static_cast<const char*>(name.c_str()), name.size() + 1);

	return o;

}
}
