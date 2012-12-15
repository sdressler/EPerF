#include "../include/EPerf/EPerfC.h"

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
    char s[256];    // Used for the configuration string

    EPerf *e = EPerfInit("eperf.db", "Fibonacci C");

    // Holds the configurations
    //EPerfKernelConf *c;

    printf("Adding kernel.\n");
    err = EPerfAddKernel(e, 0, "Fibonacci");

    // Do not forget the ERRORS!
    if (err != E_OK) {
        printf("Error: %d\n", err);
        exit(-1);
    }

    printf("Adding device\n");
    EPerfAddDevice(e, 0, "CPU");

    printf("Generating timings and datavolumes\n");

    for (i = 0; i < 4; i++) {

        // Fib number to generate
        f = (i + 1) * 10;

        // Initialize Kernel Configuration
        //c = EPerfInitKernelConf();

        // Write Configuration String
        //sprintf(s, "%d", f);
        //EPerfInsertKernelConfPair(c, "number", s);

        // Start Timer
        EPerfStartTimer(e, 0, 0);
        
        // Run Kernel
        f = fib(f);

        // Stop Timer
        EPerfStopTimer(e, 0, 0);

        // Set KDV
        EPerfAddKernelDataVolumes(e, 0, 0, 4, 4);

        printf("Fibonacci %d: %u\n", (i+1)*10, f);
    }

    printf("Printing content:\n");
    EPerfPrintResults(e);

    printf("Committing to DB\n");
    EPerfCommitToDB(e);

    return 0;

}
