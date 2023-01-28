#include "jvx_example_field2.h"
#include "jvx_dsp_base.h"
#include <stdio.h>

//#define JVX_NO_FAIL_IF_DRIVER_UNAVAIL
//#define JVX_SIMULATE_DRIVER_ONLY
// #define JVX_INSPIRE_VERBOSE_LIB 
#define JVX_NUM_VALUES_RAWRAM 0x400

#ifdef JVX_OS_LINUX

#include "jvx_driver.h"

#ifndef JVX_SIMULATE_DRIVER_ONLY
/*
#include "jvx_mydriver_libapi.h"
*/
#endif
#endif

typedef struct
{
	int file_hdl;
	jvx_example_field2_sync_str sync_copy;
} jvx_example_field2_private;

jvxDspBaseErrorType
jvx_example_field2_initCfg(jvx_example_field2* theHandle)
{
	theHandle->prv = NULL;
	theHandle->prmInit.dev_target = "/dev/jvx_example_field2";

	theHandle->prmSync.fld = NULL;
	theHandle->prmSync.fld_num = 0;
	theHandle->prmSync.fld_sz = 0;
	return JVX_DSP_NO_ERROR;
}

jvxDspBaseErrorType
jvx_example_field2_init(jvx_example_field2* theHandle)
{
	jvx_example_field2_private* prv = NULL;
	jvxDspBaseErrorType res = JVX_DSP_ERROR_WRONG_STATE;
	jvxSize nBytes = 0;
	jvxSize i;
	if (theHandle->prv == NULL)
	{
		JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(prv, jvx_example_field2_private);

		prv->sync_copy.fld = NULL;
		prv->sync_copy.fld_num = 0;
		prv->sync_copy.fld_sz = 0;

		prv->sync_copy.fld_num = JVX_NUM_VALUES_RAWRAM;
		prv->sync_copy.fld_sz = sizeof(jvxUInt32) * prv->sync_copy.fld_num;
		JVX_DSP_SAFE_ALLOCATE_FIELD_Z(prv->sync_copy.fld, jvxUInt32, prv->sync_copy.fld_num);		

		for (i = 0; i < prv->sync_copy.fld_num; i++)
		{
			prv->sync_copy.fld[i] = (jvxUInt32)i;
		}
		theHandle->prv = prv;
		res = jvx_example_field2_update(theHandle, c_false, JVX_DSP_UPDATE_SYNC);

#ifdef JVX_OS_LINUX

#ifdef JVX_DARWIN_LIB_DEBUG
		printf("::%s:: Attempting to open %s\n", __FUNCTION__, theHandle->prmInit.dev_target);
#endif
		// O_RDONLY, O_WRONLY, or O_RDWR
		// O_NONBLOCK or O_Noszi
#ifdef JVX_SIMULATE_DRIVER_ONLY
		printf("::%s:: Simulating failed open of device driver %s\n", __FUNCTION__, theHandle->prmInit.dev_target);
		prv->file_hdl = -1;
#else
		prv->file_hdl = open(theHandle->prmInit.dev_target, O_RDWR);
#endif
		if (prv->file_hdl <= 0)
		{
#ifdef JVX_DARWIN_LIB_DEBUG
			printf("::%s::  --failed\n", __FUNCTION__);
#endif

#ifdef JVX_NO_FAIL_IF_DRIVER_UNAVAIL
			printf("::%s:: Falling back to driver simulation mode - all driver API calls will be skipped.\n", __FUNCTION__);
#else
			res = JVX_DSP_ERROR_INTERNAL;
			goto leave;
#endif
		}

		if (prv->file_hdl > 0)
		{
			/*
#ifdef JVX_DARWIN_LIB_DEBUG
		    readwrite_ioctl32_VB(prv, JVX_DARWIN_IOCTL_OSZI_WRITE_MEMORY_SIZE, &theHandle->prmAsync.memory_size, "MEMORYSIZE", res);
		    readwrite_ioctl32_VB(prv, JVX_DARWIN_IOCTL_OSZI_READ_MEMORY_SIZE, &theHandle->prmAsync.memory_size, "MEMORYSIZE", res);

		    readwrite_ioctl32_VB(prv, JVX_DARWIN_IOCTL_OSZI_WRITE_IRQ_CONTROL, &theHandle->prmAsync.irq_control, "IRQCONTROL", res); // activate irqs
		    readwrite_ioctl32_VB(prv, JVX_DARWIN_IOCTL_OSZI_READ_IRQ_CONTROL, &theHandle->prmAsync.irq_control, "IRQCONTROL", res); // reread value
#else
		    readwrite_ioctl32_NVB(prv, JVX_DARWIN_IOCTL_OSZI_WRITE_MEMORY_SIZE, &theHandle->prmAsync.memory_size, "MEMORYSIZE", res);
		    readwrite_ioctl32_NVB(prv, JVX_DARWIN_IOCTL_OSZI_READ_MEMORY_SIZE, &theHandle->prmAsync.memory_size, "MEMORYSIZE", res);

		    readwrite_ioctl32_NVB(prv, JVX_DARWIN_IOCTL_OSZI_WRITE_IRQ_CONTROL, &theHandle->prmAsync.irq_control, "IRQCONTROL", res); // activate irqs
		    readwrite_ioctl32_NVB(prv, JVX_DARWIN_IOCTL_OSZI_READ_IRQ_CONTROL, &theHandle->prmAsync.irq_control, "IRQCONTROL", res); // reread value
#endif
		*/
		}
		else
		{
		    printf("::%s:: Initial read/write skipped due to unavailable driver.\n", __FUNCTION__);
		}

		// Store core handle
#else
		printf("::%s:: Warning: this module uses Linux system calls and is not conected to any hardware if not in Linux\n", __FUNCTION__);
#endif
		return JVX_DSP_NO_ERROR;
	}
#ifdef JVX_OS_LINUX
	leave :
		  JVX_DSP_SAFE_DELETE_OBJECT(prv);
#endif	
		  return res;
}

jvxDspBaseErrorType
jvx_example_field2_update(jvx_example_field2* theHandle, jvxCBool do_set, jvxUInt16 what)
{
    jvxDspBaseErrorType res = JVX_DSP_ERROR_WRONG_STATE;
	if (theHandle->prv)
	{
		jvx_example_field2_private* prv = (jvx_example_field2_private*)theHandle->prv;
		if (do_set)
		{
			switch (what)
			{
			case JVX_DSP_UPDATE_SYNC:
				res = JVX_DSP_ERROR_UNSUPPORTED;
				break;
				/*
#ifdef JVX_OS_LINUX
			case JVX_DSP_UPDATE_USER_OFFSET:
#ifdef JVX_DARWIN_LIB_DEBUG
				readwrite_ioctl32_VB(prv, JVX_DARWIN_IOCTL_OSZI_WRITE_MEMORY_SIZE, &theHandle->prmAsync.memory_size, "MEMORYSIZE", res);
#else
				readwrite_ioctl32_NVB(prv, JVX_DARWIN_IOCTL_OSZI_WRITE_MEMORY_SIZE, &theHandle->prmAsync.memory_size, "MEMORYSIZE", res);
#endif
				break;
			case JVX_DSP_UPDATE_USER_OFFSET + 1:
#ifdef JVX_DARWIN_LIB_DEBUG
				readwrite_ioctl32_VB(prv, JVX_DARWIN_IOCTL_OSZI_WRITE_OSZI_TRIGGER, &theHandle->prmAsync.busy_trigger, "TRIGGER", res);
#else
				readwrite_ioctl32_NVB(prv, JVX_DARWIN_IOCTL_OSZI_WRITE_OSZI_TRIGGER, &theHandle->prmAsync.busy_trigger, "TRIGGER", res);
#endif
				break;
			case JVX_DSP_UPDATE_USER_OFFSET + 2:
#ifdef JVX_DARWIN_LIB_DEBUG
				readwrite_ioctl32_VB(prv, JVX_DARWIN_IOCTL_OSZI_WRITE_IRQ_CONTROL, &theHandle->prmAsync.irq_control, "IRQCONTROL", res);
#else
				readwrite_ioctl32_NVB(prv, JVX_DARWIN_IOCTL_OSZI_WRITE_IRQ_CONTROL, &theHandle->prmAsync.irq_control, "IRQCONTROL", res);
#endif
				break;
#endif
			*/
			default:
				res = JVX_DSP_ERROR_INVALID_ARGUMENT;
			}
			/*
			if (prv->file_hdl > 0)
			{
			}
			else
			{
				printf("::%s:: Write of " JVX_PRINTF_CAST_SIZE " bytes to device skipped due to unavailable driver.\n", __FUNCTION__, sizeof(theHandle->prmSync));
			}*/
		}
		else
		{
			switch (what)
			{
			case JVX_DSP_UPDATE_SYNC:
				theHandle->prmSync = prv->sync_copy;
				break;
				/*
#ifdef JVX_OS_LINUX
			case JVX_DSP_UPDATE_USER_OFFSET:
#ifdef JVX_DARWIN_LIB_DEBUG
				readwrite_ioctl32_VB(prv, JVX_DARWIN_IOCTL_OSZI_READ_MEMORY_SIZE, &theHandle->prmAsync.memory_size, "MEMORYSIZE", res);
#else
				readwrite_ioctl32_NVB(prv, JVX_DARWIN_IOCTL_OSZI_READ_MEMORY_SIZE, &theHandle->prmAsync.memory_size, "MEMORYSIZE", res);
#endif
				break;
			case JVX_DSP_UPDATE_USER_OFFSET + 1:
#ifdef JVX_DARWIN_LIB_DEBUG
				readwrite_ioctl32_VB(prv, JVX_DARWIN_IOCTL_OSZI_READ_OSZI_BUSY, &theHandle->prmAsync.busy_trigger, "BUSY", res);
#else
				readwrite_ioctl32_NVB(prv, JVX_DARWIN_IOCTL_OSZI_READ_OSZI_BUSY, &theHandle->prmAsync.busy_trigger, "BUSY", res);
#endif
				break;
			case JVX_DSP_UPDATE_USER_OFFSET + 2:
#ifdef JVX_DARWIN_LIB_DEBUG
				readwrite_ioctl32_VB(prv, JVX_DARWIN_IOCTL_OSZI_READ_IRQ_CONTROL, &theHandle->prmAsync.irq_control, "IRQCONTROL", res);
#else
				readwrite_ioctl32_NVB(prv, JVX_DARWIN_IOCTL_OSZI_READ_IRQ_CONTROL, &theHandle->prmAsync.irq_control, "IRQCONTROL", res);
#endif

				break;
#endif
				*/
			default:
				res = JVX_DSP_ERROR_INVALID_ARGUMENT;
			}
			/*if (prv->file_hdl > 0)
			{
			}
			else
			{
				printf("::%s:: Read of " JVX_PRINTF_CAST_SIZE " bytes from device skipped due to unavailable driver.\n", __FUNCTION__, sizeof(theHandle->prmAsync));
			}*/
		}
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

jvxDspBaseErrorType
jvx_example_field2_terminate(jvx_example_field2* theHandle)
{
	jvxSize nBytes = 0;
#ifdef JVX_OS_LINUX
	int reti;
#endif
	if (theHandle->prv)
	{
		jvx_example_field2_private* prv = (jvx_example_field2_private*)theHandle->prv;
#ifdef JVX_OS_LINUX


		// O_RDONLY, O_WRONLY, or O_RDWR
		// O_NONBLOCK or O_Noszi
		if (prv->file_hdl > 0)
		{

#ifdef JVX_DARWIN_LIB_DEBUG
		    printf("::%s:: Attempting to close device driver\n", __FUNCTION__);
#endif
		    reti = close(prv->file_hdl);
		    if (reti != 0)
		    {
#ifdef JVX_DARWIN_LIB_DEBUG
			printf("::%s::  --failed\n", __FUNCTION__);
#endif
		    }
		}
		else
		{
		    printf("::%s:: Closing device driver skipped due to unavailable driver.\n", __FUNCTION__);
		}

#else
		//printf("::%s:: Warning: this module uses Linux system calls and is not conected to any hardware if not in Linux\n", __FUNCTION__);
#endif
		JVX_DSP_SAFE_DELETE_FIELD(prv->sync_copy.fld);
		prv->sync_copy.fld = NULL;
		prv->sync_copy.fld_num = 0;
		prv->sync_copy.fld_sz = 0;

		JVX_DSP_SAFE_DELETE_OBJECT(theHandle->prv);
		theHandle->prv = NULL;

		return JVX_DSP_NO_ERROR;

	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

/*
jvxDspBaseErrorType 
jvx_example_field2_trigger_read(jvx_example_field2* theHandle)
{
	jvxDspBaseErrorType res = JVX_DSP_ERROR_WRONG_STATE;
	jvxInt32 val = 0;
	if (theHandle->prv)
	{
		jvx_example_field2_private* prv = (jvx_example_field2_private*)theHandle->prv;
			/*
#ifdef JVX_OS_LINUX
		if (prv->file_hdl > 0)
		{
#ifdef JVX_DARWIN_LIB_DEBUG
			readwrite_ioctl32_VB(prv, JVX_DARWIN_IOCTL_OSZI_WRITE_OSZI_TRIGGER, &val, "TRIGGER0", res);
			val = 1;
			readwrite_ioctl32_VB(prv, JVX_DARWIN_IOCTL_OSZI_WRITE_OSZI_TRIGGER, &val, "TRIGGER1", res);
#else
			readwrite_ioctl32_NVB(prv, JVX_DARWIN_IOCTL_OSZI_WRITE_OSZI_TRIGGER, &val, "TRIGGER0", res);
			val = 1;
			readwrite_ioctl32_NVB(prv, JVX_DARWIN_IOCTL_OSZI_WRITE_OSZI_TRIGGER, &val, "TRIGGER1", res);
#endif
		}
#else
		//printf("::%s:: Warning: this module uses Linux system calls and is not conected to any hardware if not in Linux\n", __FUNCTION__);
#endif
			* /
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}
*/

jvxDspBaseErrorType jvx_example_field2_read_memory(jvx_example_field2* theHandle)
{
	jvxDspBaseErrorType res = JVX_DSP_ERROR_WRONG_STATE;
	jvxSize num_bytes = 0;
	jvxSize i;
	jvxSize numMin = 10;
	if (theHandle->prv)
	{
		jvx_example_field2_private* prv = (jvx_example_field2_private*)theHandle->prv;
#ifdef JVX_OS_LINUX
		if (prv->file_hdl > 0)
		{
			num_bytes = read(prv->file_hdl, prv->sync_copy.fld, prv->sync_copy.fld_sz);
			if (num_bytes != prv->sync_copy.fld_sz)
			{
				printf("Expected " JVX_PRINTF_CAST_SIZE ", got " JVX_PRINTF_CAST_SIZE " \n", prv->sync_copy.fld_sz, num_bytes);
				return JVX_DSP_ERROR_INTERNAL;
			}

#if JVX_INSPIRE_VERBOSE_LIB
			numMin = JVX_MIN(numMin, prv->sync_copy.fld_num);
			printf("[");
			for (i = 0; i < numMin; i++)
			{
				if (i != 0)
					printf(", ");
				printf("%i", prv->sync_copy.fld[i]);
			}
			printf("]\n");
#endif
		}
		else
		{
#endif
		    // Simulate varying values
		    for (i = 0; i < prv->sync_copy.fld_num; i++)
		    {
				prv->sync_copy.fld[i] = rand();
		    }
#ifdef JVX_OS_LINUX
		    printf("::%s:: Warning: driver not available, producing random data for simulation purposes.\n", __FUNCTION__);
		}
#endif
		return JVX_DSP_NO_ERROR;
	}
	return JVX_DSP_ERROR_WRONG_STATE;
}

