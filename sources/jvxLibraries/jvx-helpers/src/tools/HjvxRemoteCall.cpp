
#include "jvx-helpers.h"

jvxErrorType
jvx_run_remote_1idata_1odata(IjvxRemoteCall* cHdl, jvxSize in_ly, jvxSize in_lx, jvxDataFormat in_form, jvxSize out_ly, jvxSize out_lx, jvxDataFormat out_form, const char* action, ...)
{
	jvxErrorType resLL = JVX_NO_ERROR;
	jvxSize i;
	va_list ap;
	jvxRCOneParameter* lstParamsIn = NULL;
	jvxRCOneParameter* lstParamsOut = NULL;
	jvxErrorType errorCodeOtherSide = JVX_NO_ERROR;

	va_start(ap, action);

	cHdl->allocateParameterList(&lstParamsIn, 1);
	cHdl->allocateParameterList(&lstParamsOut, 1);

	jvx_AllocateOneRcParameter(&lstParamsIn[0], in_form, in_ly, in_lx, false);
	jvx_AllocateOneRcParameter(&lstParamsOut[0], out_form, out_ly, out_lx, false);

	for (i = 0; i < in_ly; i++)
	{
		jvxHandle* ptr = NULL;
		ptr = va_arg(ap, jvxHandle*);
		assert(ptr);
		lstParamsIn[0].content[i] = ptr;
	}
	for (i = 0; i < out_ly; i++)
	{
		jvxHandle* ptr = NULL;
		ptr = va_arg(ap, jvxHandle*);
		assert(ptr);
		lstParamsOut[0].content[i] = ptr;
	}

	resLL = cHdl->remoteRequest(lstParamsIn, 1, lstParamsOut, 1, action, JVX_SIZE_UNSELECTED, JVX_INFINITE_MS, &errorCodeOtherSide);
	for (i = 0; i < in_ly; i++)
	{
		lstParamsIn[0].content[i] = NULL;
	}
	for (i = 0; i < out_ly; i++)
	{
		lstParamsOut[0].content[i] = NULL;
	}

	jvx_DeallocateOneRcParameter(&lstParamsIn[0]);
	jvx_DeallocateOneRcParameter(&lstParamsOut[0]);
	cHdl->deallocateParameterList(lstParamsIn);
	cHdl->deallocateParameterList(lstParamsOut);

	if (errorCodeOtherSide != JVX_NO_ERROR)
	{
		resLL = errorCodeOtherSide;
	}

	va_end(ap);

	return resLL;
}

jvxErrorType
jvx_run_remote_1idata_1odata_ptrarr(IjvxRemoteCall* cHdl, jvxSize in_ly, jvxSize in_lx, jvxDataFormat in_form, jvxSize out_ly, jvxSize out_lx, jvxDataFormat out_form, const char* action, 
	jvxHandle** bufPtrIn, jvxHandle** bufPtrOut)
{
	jvxErrorType resLL = JVX_NO_ERROR;
	jvxSize i;
	jvxRCOneParameter* lstParamsIn = NULL;
	jvxRCOneParameter* lstParamsOut = NULL;
	jvxErrorType errorCodeOtherSide = JVX_NO_ERROR;

	cHdl->allocateParameterList(&lstParamsIn, 1);
	cHdl->allocateParameterList(&lstParamsOut, 1);

	jvx_AllocateOneRcParameter(&lstParamsIn[0], in_form, in_ly, in_lx, false);
	jvx_AllocateOneRcParameter(&lstParamsOut[0], out_form, out_ly, out_lx, false);

	for (i = 0; i < in_ly; i++)
	{
		jvxHandle* ptr = NULL;
		lstParamsIn[0].content[i] = bufPtrIn[i];
	}
	for (i = 0; i < out_ly; i++)
	{
		jvxHandle* ptr = NULL;
		lstParamsOut[0].content[i] = bufPtrOut[i];
	}

	resLL = cHdl->remoteRequest(lstParamsIn, 1, lstParamsOut, 1, action, JVX_SIZE_UNSELECTED, JVX_INFINITE_MS, &errorCodeOtherSide);
	for (i = 0; i < in_ly; i++)
	{
		lstParamsIn[0].content[i] = NULL;
	}
	for (i = 0; i < out_ly; i++)
	{
		lstParamsOut[0].content[i] = NULL;
	}

	jvx_DeallocateOneRcParameter(&lstParamsIn[0]);
	jvx_DeallocateOneRcParameter(&lstParamsOut[0]);
	cHdl->deallocateParameterList(lstParamsIn);
	cHdl->deallocateParameterList(lstParamsOut);

	if (errorCodeOtherSide != JVX_NO_ERROR)
	{
		resLL = errorCodeOtherSide;
	}
	return resLL;
}

jvxErrorType
jvx_run_remote_2idata_1odata(IjvxRemoteCall* cHdl, jvxSize in1_ly, jvxSize in1_lx, jvxDataFormat in1_form, jvxSize in2_ly, jvxSize in2_lx, jvxDataFormat in2_form, jvxSize out_ly, jvxSize out_lx, jvxDataFormat out_form, const char* action, ...)
{
	jvxErrorType resLL = JVX_NO_ERROR;
	jvxSize i;
	va_list ap;
	jvxRCOneParameter* lstParamsIn = NULL;
	jvxRCOneParameter* lstParamsOut = NULL;
	jvxErrorType errorCodeOtherSide = JVX_NO_ERROR;

	va_start(ap, action);

	cHdl->allocateParameterList(&lstParamsIn, 2);
	cHdl->allocateParameterList(&lstParamsOut, 1);

	jvx_AllocateOneRcParameter(&lstParamsIn[0], in1_form, in1_ly, in1_lx, false);
	jvx_AllocateOneRcParameter(&lstParamsIn[1], in2_form, in2_ly, in2_lx, false);
	jvx_AllocateOneRcParameter(&lstParamsOut[0], out_form, out_ly, out_lx, false);

	for (i = 0; i < in1_ly; i++)
	{
		jvxHandle* ptr = NULL;
		ptr = va_arg(ap, jvxHandle*);
		assert(ptr);
		lstParamsIn[0].content[i] = ptr;
	}
	for (i = 0; i < in2_ly; i++)
	{
		jvxHandle* ptr = NULL;
		ptr = va_arg(ap, jvxHandle*);
		assert(ptr);
		lstParamsIn[1].content[i] = ptr;
	}
	for (i = 0; i < out_ly; i++)
	{
		jvxHandle* ptr = NULL;
		ptr = va_arg(ap, jvxHandle*);
		assert(ptr);
		lstParamsOut[0].content[i] = ptr;
	}

	resLL = cHdl->remoteRequest(lstParamsIn, 2, lstParamsOut, 1, action, JVX_SIZE_UNSELECTED, JVX_INFINITE_MS, &errorCodeOtherSide);
	for (i = 0; i < in1_ly; i++)
	{
		lstParamsIn[0].content[i] = NULL;
	}
	for (i = 0; i < in2_ly; i++)
	{
		lstParamsIn[1].content[i] = NULL;
	}
	for (i = 0; i < out_ly; i++)
	{
		lstParamsOut[0].content[i] = NULL;
	}

	jvx_DeallocateOneRcParameter(&lstParamsIn[0]);
	jvx_DeallocateOneRcParameter(&lstParamsIn[1]);
	jvx_DeallocateOneRcParameter(&lstParamsOut[0]);
	cHdl->deallocateParameterList(lstParamsIn);
	cHdl->deallocateParameterList(lstParamsOut);

	if (errorCodeOtherSide != JVX_NO_ERROR)
	{
		resLL = errorCodeOtherSide;
	}

	va_end(ap);
	return resLL;
}

jvxErrorType
jvx_run_remote_0idata_1odata(IjvxRemoteCall* cHdl, jvxSize numOutParams, jvxSize bsizeOut, jvxDataFormat out_form, const char* action, ...)
{
	jvxErrorType resLL = JVX_NO_ERROR;
	jvxSize i;
	va_list ap;
	jvxRCOneParameter* lstParamsOut = NULL;
	jvxErrorType errorCodeOtherSide = JVX_NO_ERROR;

	va_start(ap, action);
	cHdl->allocateParameterList(&lstParamsOut, 1);
	jvx_AllocateOneRcParameter(&lstParamsOut[0], out_form, numOutParams, bsizeOut, false);

	for (i = 0; i < numOutParams; i++)
	{
		jvxHandle* ptr = va_arg(ap, jvxHandle*);
		assert(ptr);
		lstParamsOut[0].content[i] = ptr;
	}

	resLL = cHdl->remoteRequest(NULL, 0, lstParamsOut, 1, action, JVX_SIZE_UNSELECTED, JVX_INFINITE_MS, &errorCodeOtherSide);
	for (i = 0; i < numOutParams; i++)
	{
		lstParamsOut[0].content[i] = NULL;
	}

	jvx_DeallocateOneRcParameter(&lstParamsOut[0]);
	cHdl->deallocateParameterList(lstParamsOut);

	if (errorCodeOtherSide != JVX_NO_ERROR)
	{
		resLL = errorCodeOtherSide;
	}
	return resLL;
}

jvxErrorType
jvx_run_remote_0idata_0odata(IjvxRemoteCall* cHdl, const char* action)
{
	jvxErrorType resLL = JVX_NO_ERROR;
	jvxErrorType errorCodeOtherSide = JVX_NO_ERROR;
	resLL = cHdl->remoteRequest(NULL, 0, NULL, 0, action, JVX_SIZE_UNSELECTED, JVX_INFINITE_MS, &errorCodeOtherSide);

	if (errorCodeOtherSide != JVX_NO_ERROR)
	{
		resLL = errorCodeOtherSide;
	}
	return resLL;
}

