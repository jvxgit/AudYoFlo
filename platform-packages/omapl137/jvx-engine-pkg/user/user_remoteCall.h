/*
 * ==============================================
 * Filename: genericIp.h
 * ==============================================
 * Purpose: API specification for high level remote calls.
 *			COPYRIGHT HK, Javox, 2011
 */

#ifndef _USER_REMOTECALLS_H__
#define _USER_REMOTECALLS_H__

#include "TjvxTypes.h"
#include "jvx_remote_call.h"

// Function to "prepare" for a remote call coming later. Return 1 if want to continue and 0 if reject
jvxDspBaseErrorType user_reportActionRequest(char* actionName, jvxInt32 numIn, jvxInt32 numOut, jvxInt16 procId);

// Actual remote call trigger function. You find the input and output parameters in the specified parameters.
jvxDspBaseErrorType user_reportActionProcess(const char* description, jvxRCOneParameter* paramsIn, jvxSize numParamsIn, jvxRCOneParameter* paramsOut, jvxSize numParamsOut, jvxSize processingId);

// Function called in case of an error in remoteCall management
void user_reportErrorRemoteCall(int localError);

#endif
