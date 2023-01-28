#ifndef __JVX_DRIVER_H__
#define __JVX_DRIVER_H__

#include <linux/ioctl.h>

#define readwrite_ioctl32_VB(prv_fhdl, request, val, descr, res)	\
    { \
	int suc = -1; \
	res = JVX_DSP_NO_ERROR; \
	printf("::%s:: Attempting to exchange " JVX_PRINTF_CAST_SIZE " bytes for %s from device via ioctl id=%lu\n", \
	       __FUNCTION__, sizeof(jvxInt32), descr, request);		\
	suc = ioctl(prv_fhdl, request,  val); \
	if (suc != 0) \
	{ \
	  printf("::%s::  --failed, ioctl returned <%i> \n", __FUNCTION__, (int)suc); \
	    res = JVX_DSP_ERROR_INTERNAL; \
	} \
    }

#define readwrite_ioctl32_NVB(prv_fhdl, request, val, descr, res)\
    { \
	int suc = -1; \
	res = JVX_DSP_NO_ERROR; \
	suc = ioctl(prv_fhdl, request,  val); \
	if (suc != 0) \
	{ \
	    res = JVX_DSP_ERROR_INTERNAL; \
	} \
    }

#endif
