#include "../include/EPerf/EPerf.h"

#include <iostream>
#include <sstream>

using namespace ENHANCE;
using namespace std;

unsigned int fib(unsigned int n) {
    if (n < 2) {
        return n;
    } else {
        return fib(n-1) + fib(n-2);
    }
}

int main(void) {

    EPerf e;

    cout << "Adding kernel.\n";
    e.addKernel(0, "Fibonacci");
    
    cout << "Adding device\n";
    e.addDevice(0, "CPU");

    cout << "Generating timings and datavolumes\n";

    unsigned int f;
    for (int i = 0; i < 4; i++) {

        // Fib number to generate
        f = (i + 1) * 10;
        
        // Initialize a Kernel Configuration
        EPerfKernelConf c;

        // Write Kernel Configuration
        std::stringstream ss;
        ss << f;
        c.insertKernelConfPair("number", ss.str());

        // Activate Kernel Configuration
       // e.setKernelConf(0, c);

        // Set KDV
        e.addKernelDataVolumes(0, 0, 4, 4);

        // Start Timer
        e.startTimer(0, 0, c);

        // Run Kernel
        f = fib(f);
        sleep(1);

        // Stop Timer
        e.stopTimer(0, 0);

        cout << "Fibonacci " << (i+1)*10 << ": " << f << "\n";
    }


    cout << "Printing content:\n";
    cout << e;

    cout << "Committing to DB\n";
    e.commitToDB();

    return 0;

}
