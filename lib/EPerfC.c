#include "../include/EPerf/EPerfC.h"
#include "../include/EPerf/EPerf.h"

EPerf* EPerfInit() {
	return cpp_callback_EPerfInit();
}

EPerfKernelConf* EPerfInitKernelConf() {
    return cpp_callback_EPerfInitKernelConf();
}

int EPerfAddKernel(EPerf *e, int ID, const char *kName) {
	return cpp_callback_EPerfAddKernel(e, ID, kName);
}

int EPerfAddDevice(EPerf *e, int ID, const char *dName) {
	return cpp_callback_EPerfAddDevice(e, ID, dName);
}

int EPerfAddSubDeviceToDevice(EPerf *e, const int ID, const int sID) {
	return cpp_callback_EPerfAddSubDeviceToDevice(e, ID, sID);
}

int EPerfStartTimer(EPerf *e, int KernelID, int DeviceID) {
	return cpp_callback_EPerfStartTimer(e, KernelID, DeviceID);
}

int EPerfStopTimer(EPerf *e, int KernelID, int DeviceID) {
	return cpp_callback_EPerfStopTimer(e, KernelID, DeviceID);
}

int EPerfAddKernelDataVolumes(EPerf *e, int KernelID, int DeviceID, int64_t inBytes, int64_t outBytes) {
	return cpp_callback_EPerfAddKernelDataVolumes(e, KernelID, DeviceID, inBytes, outBytes);
}

int EPerfInsertKernelConfPair(EPerfKernelConf *c, const char *key, const char *value) {
    return cpp_callback_EPerfInsertKernelConfPair(c, key, value);
}

int EPerfSetKernelConf(EPerf *e, int KernelID, EPerfKernelConf *c) {
    return cpp_callback_EPerfSetKernelConf(e, KernelID, c);
}

void EPerfCommitToDB(EPerf *e) {
	cpp_callback_EPerfCommitToDB(e);
}

void EPerfPrintResults(EPerf *e) {	cpp_callback_EPerfPrintResults(e);
}
