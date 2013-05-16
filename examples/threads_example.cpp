#include "../include/EPerf/EPerf.h"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>

#include <omp.h>

#define THREADS 32

using namespace ENHANCE;

int main(void) {

	omp_set_num_threads(THREADS);

	std::cout << "Max threads: " << THREADS << "\n";

	// Create ePerF object
	EPerf e("thread.db", "Threads");

	// Add kernel
    e.addKernel(1024, "Sleep");

	// Add device
	e.addDevice(1024, "CPU");

	std::cout << "Starting\n";

    #pragma omp parallel
    {

    #pragma omp for
    for (int i = 0; i < 64; i++) {

        // Start Timer
        e.startTimer(1024, 1024);

        usleep(1);

        // Stop Timer
        e.stopTimer(1024, 1024);
    }

    }

	std::cout << "Done.\n";

    e.commitToDB();

	return 0;
}
