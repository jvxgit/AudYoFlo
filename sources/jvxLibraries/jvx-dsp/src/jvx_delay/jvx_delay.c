#include "jvx_delay/jvx_delay.h"
#include "jvx_circbuffer/jvx_circbuffer.h"

typedef struct
{
	jvx_delay_prmInit prmInit_copy;
	jvx_delay_prmSync prmSync_copy;

	jvxSize nchan_delay_buffer_sz;
	jvx_circbuffer* nchan_delay_buffer;

}jvx_delay_private;

// =============================================================================================

jvxDspBaseErrorType
jvx_delay_initCfg(jvx_delay* hdl)
{
	hdl->prmInit.delay_max = 1000;
	hdl->prmInit.numChannels = 2;
	hdl->prmInit.bSize = 1024;

	hdl->prmSync.delay_current = 0;
	hdl->prv = NULL;

	return JVX_NO_ERROR;
}

jvxDspBaseErrorType
jvx_delay_prepare(jvx_delay* hdl)
{
	if (hdl->prv != NULL)
		return JVX_ERROR_WRONG_STATE;

	jvx_delay_private* newHdl = NULL;
	JVX_DSP_SAFE_ALLOCATE_OBJECT_Z(newHdl, jvx_delay_private);

	newHdl->prmInit_copy = hdl->prmInit;

	// Buffers size to cover maximum delay
	newHdl->nchan_delay_buffer_sz = newHdl->prmInit_copy.delay_max + newHdl->prmInit_copy.bSize;

	// ALlocate circular buffer
	jvx_circbuffer_allocate(&newHdl->nchan_delay_buffer, newHdl->nchan_delay_buffer_sz, 1, newHdl->prmInit_copy.numChannels);

	jvx_circbuffer_fill(newHdl->nchan_delay_buffer, 0, newHdl->prmInit_copy.delay_max);
	newHdl->prmSync_copy.delay_current = newHdl->prmInit_copy.delay_max;

	// Desired delay_val
	newHdl->prmSync_copy.delay_current = hdl->prmSync.delay_current;

	hdl->prv = newHdl;
	return JVX_NO_ERROR;
}

jvxDspBaseErrorType
jvx_delay_process(jvx_delay* hdl, jvxData** in, jvxData** out)
{
	jvxSize delay_val = 0;
	if (hdl->prv == NULL)
		return JVX_ERROR_WRONG_STATE;

	jvx_delay_private* workHdl = (jvx_delay_private * )hdl->prv;

	jvx_circbuffer_write_update(workHdl->nchan_delay_buffer, (const jvxData**)in, workHdl->prmInit_copy.bSize);
	delay_val = JVX_MIN(hdl->prmSync.delay_current, workHdl->prmInit_copy.delay_max);
	delay_val = workHdl->prmInit_copy.delay_max - delay_val;
	jvx_circbuffer_read_update_offset(workHdl->nchan_delay_buffer, out, workHdl->prmInit_copy.bSize, delay_val);

	return JVX_NO_ERROR;
}

jvxDspBaseErrorType
jvx_delay_postprocess(jvx_delay* hdl)
{
	if (hdl->prv == NULL)
		return JVX_ERROR_WRONG_STATE;

	jvx_delay_private* workHdl = (jvx_delay_private *)hdl->prv;

	jvx_circbuffer_deallocate(workHdl->nchan_delay_buffer);

	JVX_DSP_SAFE_DELETE_OBJECT(workHdl);

	hdl->prv = NULL;
	return JVX_NO_ERROR;
}

// ================================================================================================
