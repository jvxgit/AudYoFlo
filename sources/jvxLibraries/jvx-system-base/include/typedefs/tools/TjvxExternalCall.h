#ifndef __TJVXEXTERNALCALL_H__
#define __TJVXEXTERNALCALL_H__

JVX_INTERFACE IjvxExternalCall;
JVX_INTERFACE IjvxExternalCallTarget;

//! Unspecific typedef for external (Matlab) data handles
typedef void jvxExternalDataType;

/**
 * Prototype function for C function calls from Matlab.
 *///=====================================================
typedef jvxErrorType JVX_CALLINGCONVENTION (*jvxExternalFunction)(IjvxExternalCall* theMatCallHandler, IjvxExternalCallTarget* fObject, int nlhs, jvxExternalDataType **plhs,
 				  int nrhs, const jvxExternalDataType **prhs, int offset_nlhs, int offset_nrhs );


#endif
