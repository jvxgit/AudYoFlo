#ifndef __JVX_DRIVER_H__
#define __JVX_DRIVER_H__

#define readwrite_ioctl32_VB(prv_fhdl, request, val, descr, res)	\
    { \
	int suc = -1; \
	res = JVX_DSP_NO_ERROR; \
	printf("::%s:: Attempting to exchange " JVX_PRINTF_CAST_SIZE " bytes for %s with device (via ioctl) - SIMULATION ONLY WINDOWS\n", \
	       __FUNCTION__, sizeof(jvxInt32), descr);		\
    }

#define readwrite_ioctl32_NVB(prv_fhdl, requeste, val, descr, res)\
    { \
	int suc = -1; \
	res = JVX_DSP_NO_ERROR; \
    }

#endif
