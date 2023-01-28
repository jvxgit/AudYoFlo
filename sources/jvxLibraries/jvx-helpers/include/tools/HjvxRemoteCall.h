#ifndef __HJVXREMOTECALL_H__
#define __HJVXREMOTECALL_H__

jvxErrorType
jvx_run_remote_1idata_1odata(IjvxRemoteCall* cHdl, jvxSize in_ly, jvxSize in_lx, jvxDataFormat in_form, jvxSize out_ly, jvxSize out_lx, jvxDataFormat out_form, const char* action, ...);

jvxErrorType
jvx_run_remote_2idata_1odata(IjvxRemoteCall* cHdl, jvxSize in1_ly, jvxSize in1_lx, jvxDataFormat in1_form, jvxSize in2_ly, jvxSize in2_lx, jvxDataFormat in2_form, jvxSize out_ly, jvxSize out_lx, jvxDataFormat out_form, const char* action, ...);

jvxErrorType
jvx_run_remote_0idata_1odata(IjvxRemoteCall* cHdl, jvxSize numOutParams, jvxSize bsizeOut, jvxDataFormat out_form, const char* action, ...);

jvxErrorType
jvx_run_remote_0idata_0odata(IjvxRemoteCall* cHdl, const char* action);

// =============================================================================================

jvxErrorType
jvx_run_remote_1idata_1odata_ptrarr(IjvxRemoteCall* cHdl, jvxSize in_ly, jvxSize in_lx, jvxDataFormat in_form, jvxSize out_ly, jvxSize out_lx, jvxDataFormat out_form, const char* action,
	jvxHandle** bufPtrIn, jvxHandle** bufPtrOut);

#endif
