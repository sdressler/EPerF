#include "../include/EPerf/EPerf.h"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>

#include <omp.h>

using namespace ENHANCE;

unsigned int fib(unsigned int n) {
	if (n < 2) {
		return n;
	} else {
		return fib(n-1) + fib(n-2);
	}
}

int main(void) {

	int threads = omp_get_max_threads();
	std::cout << "Max threads: " << threads << "\n";

	// Create ePerF object
	EPerf e;

	// Add kernel
    e.addKernel(0, "Fibonacci");

	// Add device
	e.addDevice(0, "Multicore CPU");

	std::cout << "Starting\n";

    unsigned int f;

    #pragma omp parallel
    {

    #pragma omp for
    for (int i = 0; i < 4; i++) {

        // Fib number to generate
//        f = (i + 1) * 20;
  
        f = 43;

        // Initialize a Kernel Configuration
        EPerfKernelConf c;

        // Write Kernel Configuration
        std::stringstream ss;
        ss << f;
        c.insertKernelConfPair("number", ss.str());

        // Set KDV
        e.addKernelDataVolumes(0, 0, 4, 4);

        // Start Timer
        e.startTimer(0, 0, c);

        // Run Kernel
        f = fib(f);

        // Stop Timer
        e.stopTimer(0, 0);

//        std::cout << "Fibonacci " << (i+1)*10 << ": " << f << "\n";
    }

    }

	std::cout << "Done.\n";

	std::cout << e;

    e.commitToDB();

	return 0;
}
