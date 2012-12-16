#include "../include/EPerf/EPerf.h"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>

#include <omp.h>

using namespace ENHANCE;

int main(void) {

	int threads = omp_get_max_threads();
	std::cout << "Max threads: " << threads << "\n";

	// Create ePerF object
	EPerf e("eperf.db", "Threads");

	// Add kernel
    e.addKernel(0, "Sleep");

	// Add device
	e.addDevice(0, "CPU");

	std::cout << "Starting\n";

    #pragma omp parallel
    {

    #pragma omp for
    for (int i = 0; i < 16; i++) {

        // Start Timer
        e.startTimer(0, 0);

        sleep(1);

        // Stop Timer
        e.stopTimer(0, 0);
    }

    }

	std::cout << "Done.\n";

    e.commitToDB();

	return 0;
}
