#include "../include/eperf/ceperf.h"
#include "../include/eperf/eperf.h"

EPerf* EPerfInit() {
	return cpp_callback_EPerfInit();
}

int EPerfAddKernel(EPerf *e, int ID, const char *kName) {
	return cpp_callback_EPerfAddKernel(e, ID, kName);
}

int EPerfAddDevice(EPerf *e, int ID, const char *dName) {
	return cpp_callback_EPerfAddDevice(e, ID, dName);
}

int EPerfStartTimer(EPerf *e, int KernelID, int DeviceID) {
	return cpp_callback_EPerfStartTimer(e, KernelID, DeviceID);
}

int EPerfStopTimer(EPerf *e, int KernelID, int DeviceID) {
	return cpp_callback_EPerfStopTimer(e, KernelID, DeviceID);
}

int EPerfAddKernelDataVolumes(EPerf *e, int KernelID, int DeviceID, long long inBytes, long long outBytes) {
	return cpp_callback_EPerfAddKernelDataVolumes(e, KernelID, DeviceID, inBytes, outBytes);
}

void EPerfPrintResults(EPerf *e) {
	cpp_callback_EPerfPrintResults(e);
}
