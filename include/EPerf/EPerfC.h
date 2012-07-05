/**
 *
 * C wrapper for ePerF (which is written in C++).
 *
 * @author	Sebsatian Dressler
 * @date	2012-05-25
 *
 * \addtogroup CePerF
 * @{
 *
 * */

#include "EPerfErrors.h"
#include "EPerf.h"
#include "EPerfKernelConf.h"

#ifndef C_EPERF_H
#define C_EPERF_H

#ifdef __cplusplus

using namespace ENHANCE;

extern "C" {
#endif

/**
 *
 * Initialises the EPerf object.
 *
 * @retval EPerf Pointer to a valid EPerf object or NULL if something went wrong
 *
 * */
EPerf* EPerfInit();
EPerf* cpp_callback_EPerfInit();

EPerfKernelConf* EPerfInitKernelConf();
EPerfKernelConf* cpp_callback_EPerfInitKernelConf();

/**
 * Adds a new kernel to the framework. The kernel is only added if the ID does
 * not yet exist.
 *
 * @param[in] e Pointer to the instantiated EPerf object
 * @param[in] ID The unique ID of the kernel
 * @param[in] kName The (optional) name of the kernel
 * @retval #ERR E_OK for success, E_DUPK if the ID already exists.
 * */
int EPerfAddKernel(EPerf *e, int ID, const char *kName);
int cpp_callback_EPerfAddKernel(EPerf *e, int ID, const char *kName);

/**
 * Adds a new device to the framework.
 *
 * @param[in] e Pointer to the instantiated EPerf object
 * @param[in] ID The unique ID of the device
 * @param[in] dName The (optional) name of the device
 * @retval #ERR E_OK for success, E_DUPD if the ID already exists.
 * */
int EPerfAddDevice(EPerf *e, int ID, const char *dName);
int cpp_callback_EPerfAddDevice(EPerf *e, int ID, const char *dName);

/**
 * Adds a subdevice to an existing device.
 *
 * @param[in] e Pointer to the instantiated EPerf object
 * @param[in] ID The unique ID of the device where the subdevice shall be added
 * @param[in] sID The unique ID of the subdevice to be added
 * @retval #ERR
 * 			E_OK for success,
 * 			E_NOD if one of the devices does not exist,
 * 			E_EQU if both ID's are the same
 * */
int EPerfAddSubDeviceToDevice(EPerf *e, const int ID, const int sID);
int cpp_callback_EPerfAddSubDeviceToDevice(EPerf *e, const int ID, const int sID);

/**
 * Starts the time measurement for a specific kernel / device combination
 *
 * @param[in] e Pointer to the instantiated EPerf object
 * @param[in] KernelID The ID of the kernel
 * @param[in] DeviceID The ID of the device
 * @retval #ERR
 * 		E_OK for success,
 * 		E_NOK if the kernel was not found,
 * 		E_NOD if the device was not found
 * */
int EPerfStartTimer(EPerf *e, int KernelID, int DeviceID);
int EPerfStartTimerWithConfig(EPerf *e, int KernelID, int DeviceID, EPerfKernelConf *c);
int cpp_callback_EPerfStartTimer(EPerf *e, int KernelID, int DeviceID, EPerfKernelConf *c);

/**
 * Stops the time measurement for a specific kernel / device combination
 *
 * @param[in] e Pointer to the instantiated EPerf object
 * @param[in] KernelID The ID of the kernel
 * @param[in] DeviceID The ID of the device
 * @retval #ERR
 * 		E_OK for success,
 * 		E_NOK if the kernel was not found,
 * 		E_NOD if the device was not found,
 * 		E_TMR if the start time was not found,
 * 		E_RES if the result is wrong (i.e. time difference negative)
 * */
int EPerfStopTimer(EPerf *e, int KernelID, int DeviceID);
int cpp_callback_EPerfStopTimer(EPerf *e, int KernelID, int DeviceID);

/**
 * Registers the data to be transferred for a specific kernel / device combination
 *
 * @param[in] e Pointer to the instantiated EPerf object
 * @param[in] KernelID The ID of the kernel
 * @param[in] DeviceID The ID of the device
 * @param[in] inBytes  The number of Bytes to be transferred from <b>host to device</b>
 * @param[in] outBytes The number of Bytes to be transferred from <b>device to host</b>
 * @retval #ERR
 * 		E_OK for success,
 * 		E_NOK if the kernel was not found,
 * 		E_NOD if the device was not found
 * */
int EPerfAddKernelDataVolumes(EPerf *e, int KernelID, int DeviceID, int64_t inBytes, int64_t outBytes);
int cpp_callback_EPerfAddKernelDataVolumes(EPerf *e, int KernelID, int DeviceID, int64_t inBytes, int64_t outBytes);

int EPerfInsertKernelConfPair(EPerfKernelConf *c, const char *key, const char *value);
int cpp_callback_EPerfInsertKernelConfPair(EPerfKernelConf *c, const char *key, const char *value);

/*
int EPerfSetKernelConf(EPerf *e, int KernelID, EPerfKernelConf *c);
int cpp_callback_EPerfSetKernelConf(EPerf *e, int KernelID, EPerfKernelConf *c);
*/

/**
 * Dumps the current content of the framework.
 *
 * @param[in] e Pointer to the instantiated EPerf object
 *
 * */
void EPerfPrintResults(EPerf *e);
void cpp_callback_EPerfPrintResults(EPerf *e);

/**
 * Write the results to a Berkeley DB. If the DB not exists, it is created.
 *
 * */
void EPerfCommitToDB(EPerf *e);
void cpp_callback_EPerfCommitToDB(EPerf *e);

#ifdef __cplusplus
}
#endif

#endif
/** @} */
