#ifndef EPERF_KERNEL_H
#define EPERF_KERNEL_H

#include <iostream>
#include <string>

namespace ENHANCE {
class EPerfKernel {
private:
	std::string name;

public:
	EPerfKernel(std::string s) {
		name = s;
	}

	friend std::ostream& operator<<(std::ostream &out, const EPerfKernel &k) {
		out << k.name;
		return out;
	}
};
}

#endif /* EPERF_KERNEL_H */
