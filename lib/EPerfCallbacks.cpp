#include "../include/eperf/EPerf.h"
#include "../include/eperf/EPerfC.h"
#include "../include/eperf/EPerfErrors.h"

#include <stdexcept>
#include <iostream>

using namespace ENHANCE;

EPerf* cpp_callback_EPerfInit() {
	EPerf *e;

	try {
		e = new EPerf();
	} catch (...) {
		return NULL;
	}

	return e;
}

int cpp_callback_EPerfAddKernel(EPerf *e, int ID, const char *kName) {
	try {
		e->addKernel(ID, std::string(kName));
	} catch (std::runtime_error &e) {
		return E_DUPK;
	}
	return E_OK;
}

int cpp_callback_EPerfAddDevice(EPerf *e, int ID, const char *dName) {
	try {
		e->addDevice(ID, std::string(dName));
	} catch (std::runtime_error &e) {
		return E_DUPD;
	}
	return E_OK;
}

int cpp_callback_EPerfAddSubDeviceToDevice(EPerf *e, const int ID, const int sID) {
	try {
		e->addSubDeviceToDevice(ID, sID);
	} catch (std::runtime_error &e) {
		return E_NOD;
	} catch (std::invalid_argument &e) {
		return E_EQU;
	}
	return E_OK;
}

int cpp_callback_EPerfStartTimer(EPerf *e, int KernelID, int DeviceID) {
	try {
		e->startTimer(KernelID, DeviceID);
	} catch (std::invalid_argument &e) {
		// Decode exception
		std::string what = e.what();
		if (what.find("Kernel")) {
			return E_NOK;
		} else if (what.find("Device")) {
			return E_NOD;
		} else {
			// Fatal error
			std::cerr << "Fatal error. Exiting.\n";
			std::terminate();
		}
	}
	return E_OK;
}

int cpp_callback_EPerfStopTimer(EPerf *e, int KernelID, int DeviceID) {
	try {
		e->stopTimer(KernelID, DeviceID);
	} catch (std::invalid_argument &e) {
		// Decode exception
		std::string what = e.what();
		if (what.find("Kernel")) {
			return E_NOK;
		} else if (what.find("Device")) {
			return E_NOD;
		} else {
			// Fatal error
			std::cerr << "Fatal error. Exiting.\n";
			std::terminate();
		}
	} catch (std::runtime_error &e) {
		return E_TMR;
	} catch (std::logic_error &e) {
		return E_RES;
	}
	return E_OK;
}

int cpp_callback_EPerfAddKernelDataVolumes(EPerf *e, int KernelID, int DeviceID, int64_t inBytes, int64_t outBytes) {
	try {
		e->addKernelDataVolumes(KernelID, DeviceID, inBytes, outBytes);
	} catch (std::invalid_argument &e) {
		// Decode exception
		std::string what = e.what();
		if (what.find("Kernel")) {
			return E_NOK;
		} else if (what.find("Device")) {
			return E_NOD;
		} else {
			// Fatal error
			std::cerr << "Fatal error. Exiting.\n";
			std::terminate();
		}
	}
	return E_OK;
}

void cpp_callback_EPerfCommitToDB(EPerf *e) {
	e->commitToDB();
}

void cpp_callback_EPerfPrintResults(EPerf *e) {
	std::cout << *e;
}
