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

	// Add kernels, one for each thread
	for (int i = 0; i < threads; i++) {
		std::stringstream ss;
		ss << "Fibonacci Kernel, Thread: " << i;
		e.addKernel(i, ss.str());
	}

	// Add device
	e.addDevice(0, "Multicore CPU");

	// Seed random number generator
	srand(time(NULL));

	std::cout << "Starting\n";

	#pragma omp parallel for
	for (int i = 0; i < 1000; i++) {
		int tID = omp_get_thread_num();
		std::cout << "Thread ID: " << tID << "\n";
		int f = rand() % 30;
//		std::cout << "Thread ID: " << tID << " Fib: " << f << "\n";
/* ==== ePerF insertion point ==== */
		e.startTimer(tID, 0);
		f = fib(f);
		e.stopTimer(tID, 0);
/* =============================== */
	}

	std::cout << "Done.\n";

	std::cout << e;

	return 0;
}
