/**
 *
 * C wrapper for ePerF Affinity module
 *
 * @author	Sebastian Dressler
 * @date	2012-06-02
 *
 * \addtogroup CePerF
 * @{
 *
 * */

#include "EPerfAffinity.h"
#include "EPerfErrors.h"

#ifndef C_EPERF_AFFINITY_H
#define C_EPERF_AFFINITY_H

#ifdef __cplusplus

using namespace ENHANCE;

extern "C" {
#endif

/**
 *
 * Initialize the EPerfAffinity affinity object
 *
 * @retval EPerfAffinity Pointer to a valid EPerfAffinity object or NULL if something went wrong
 *
 * */
EPerfAffinity* EPerfAffinityInit();
EPerfAffinity* cpp_callback_EPerfAffinityInit();

#ifdef __cplusplus
}
#endif

#endif
/** @ */
