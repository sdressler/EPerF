#include "../include/eperf/ceperf.h"

#include <stdio.h>
#include <stdlib.h>

unsigned int fib(unsigned int n) {
	if (n < 2) {
		return n;
	} else {
		return fib(n-1) + fib(n-2);
	}
}

int main(void) {

	unsigned int f;
	int i, err;

	EPerf *e = EPerfInit();

	printf("Adding kernel.\n");
	err = EPerfAddKernel(e, 0, "Fibonacci 10");
	err = EPerfAddKernel(e, 1, "Fibonacci 20");
	err = EPerfAddKernel(e, 2, "Fibonacci 30");
  	err = EPerfAddKernel(e, 3, "Fibonacci 40");

	if (err != E_OK) {
		printf("Error: %d\n", err);
		exit(-1);
	}

	printf("Adding device\n");
	EPerfAddDevice(e, 0, "Test Device");

	printf("Generating timings and datavolumes\n");

	for (i = 0; i < 4; i++) {

		EPerfAddKernelDataVolumes(e, 1, 0, 1024, 512);

		EPerfStartTimer(e, i, 0);
		sleep(1);
		f = fib((i + 1) * 10);
		EPerfStopTimer(e, i, 0);

		printf("Fibonacci %d: %u\n", (i+1)*10, f);
	}

	printf("Printing content:\n");
	EPerfPrintResults(e);
	
	return 0;

}
