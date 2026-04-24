#ifndef __TJVXEXTERNALCALL_H__
#define __TJVXEXTERNALCALL_H__

JVX_INTERFACE IjvxExternalCall;
JVX_INTERFACE IjvxExternalCallTarget;

//! Unspecific typedef for external (Matlab) data handles
typedef void jvxExternalDataType;

#define EXTERNAL_INTERFACE_MESSAGE_OPTIONS(X) \
    X(external_if_info) \
    X(external_if_warning) \
    X(external_if_error) 

enum external_if_message_type
{
    EXTERNAL_INTERFACE_MESSAGE_OPTIONS(GENERATE_ENUM)
};

/**
 * Prototype function for C function calls from Matlab.
 *///=====================================================
typedef jvxErrorType JVX_CALLINGCONVENTION (*jvxExternalFunction)(IjvxExternalCall* theMatCallHandler, IjvxExternalCallTarget* fObject, int nlhs, jvxExternalDataType **plhs,
 				  int nrhs, const jvxExternalDataType **prhs, int offset_nlhs, int offset_nrhs );


#endif
