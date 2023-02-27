#include "HjvxDataLinkDescriptor.h"
#include "jvx_misc/jvx_allocate_fields.h"
#include "jvx-helpers.h"

jvxErrorType
jvx_initDataLinkDescriptor(jvxLinkDataDescriptor* theData)
{
	if (theData)
	{
		jvx_neutralDataLinkDescriptor(theData, true);
		jvx_neutralDataLinkDescriptor(theData, false);
		
		theData->con_link.uIdConn = JVX_SIZE_UNSELECTED;
		theData->con_link.master = NULL;
		theData->con_link.connect_to = NULL;
		theData->con_link.connect_from = NULL;
		theData->con_link.tp_master.reset();

		// theData->con_link.address_flags = JVX_LINKDATA_ADDRESS_FLAGS_NONE;
		//theData->link.allows_mt = false;

#ifndef JVX_COMPILE_SMALL
		theData->con_pipeline.do_lock = NULL;
		theData->con_pipeline.do_try_lock = NULL;
		theData->con_pipeline.do_unlock = NULL;
		theData->con_pipeline.lock_hdl = NULL;
		theData->con_pipeline.reserve_buffer_pipeline_stage = NULL;
#endif

		theData->con_pipeline.num_additional_pipleline_stages = 0;
		theData->con_pipeline.idx_stage_ptr = NULL;

		theData->con_sync.reserve_timestamp = NULL;
		theData->con_sync.type_timestamp = 0;

		theData->con_compat.idx_receiver_to_sender = 0;

#ifndef JVX_COMPILE_SMALL
		theData->admin.tStamp_us = JVX_SIZE_UNSELECTED;
		theData->admin.frames_lost_since_last_time = 0;
#endif
		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
};

jvxErrorType
jvx_terminateDataLinkDescriptor(jvxLinkDataDescriptor* theData)
{
	if (theData)
	{
		jvx_neutralDataLinkDescriptor(theData, true);
		jvx_neutralDataLinkDescriptor(theData, false);
		assert(theData->con_data.buffers == NULL);
		assert(theData->con_compat.from_receiver_buffer_allocated_by_sender == NULL);

		theData->con_link.connect_from = NULL;
		theData->con_link.connect_to = NULL;
		theData->con_link.master = NULL;
		theData->con_link.uIdConn = JVX_SIZE_UNSELECTED;

		// theData->con_link.address_flags = JVX_LINKDATA_ADDRESS_FLAGS_NONE;
		//theData->link.allows_mt = false;
		theData->con_link.tp_master.reset();

#ifndef JVX_COMPILE_SMALL
		theData->con_pipeline.do_lock = NULL;
		theData->con_pipeline.do_try_lock = NULL;
		theData->con_pipeline.do_unlock = NULL;
		theData->con_pipeline.lock_hdl = NULL;
		theData->con_pipeline.reserve_buffer_pipeline_stage = NULL;
#endif
		theData->con_pipeline.num_additional_pipleline_stages = 0;
		theData->con_pipeline.idx_stage_ptr = NULL;

		theData->con_sync.reserve_timestamp = NULL;
		theData->con_sync.type_timestamp = 0;

		theData->con_compat.idx_receiver_to_sender = 0;

		return(JVX_NO_ERROR);
	}
	return(JVX_ERROR_INVALID_ARGUMENT);
};

jvxErrorType
jvx_allocateDataLinkDescriptor_oneExtBuffer(jvxLinkDataDescriptor_buf* theData, jvxSize idxBuf, jvxSize idxChan, 
	jvxSize buffersize, jvxSize szElem, jvxByte*** raw, jvxByte*** net, jvxSize** offset, jvxSize* sz)
{

	jvx_allocateField_c_a((jvxHandle**)&raw[idxBuf][idxChan], buffersize,
		szElem, (theData->prepend + theData->append + theData->align),
		sz, theData->f_alloc, theData->f_priv);
	intptr_t ptrI = (intptr_t)((jvxByte*)raw[idxBuf][idxChan]);
	intptr_t dec = 0;
	intptr_t inc = theData->prepend;
	if (theData->align)
	{
		// 0x12345 align 4: 0x12345 + 4 - 1
		inc += (theData->align - 1);

		// Here is my attempt: 
		// convert pointer to int, do the align and compute the pointer decrement
		// and finally apply the decrement
		dec = (ptrI + inc) % theData->align;
	}
	offset[idxBuf][idxChan] = inc - dec;
	net[idxBuf][idxChan] =
		((jvxByte*)raw[idxBuf][idxChan] + offset[idxBuf][idxChan]);
	return JVX_NO_ERROR;
}

jvxErrorType
jvx_deallocateDataLinkDescriptor_oneExtBuffer(jvxLinkDataDescriptor_buf* theData, jvxSize idxBuf, jvxSize idxChan, jvxByte*** raw, jvxByte*** net, jvxSize** offset)
{
	if (theData)
	{
		jvx_deallocateField_c_a((jvxHandle**)&raw[idxBuf][idxChan], theData->f_dealloc, theData->f_priv);
	}
	else
	{
		jvx_deallocateField_c_a((jvxHandle**)&raw[idxBuf][idxChan], NULL, NULL);
	}
	raw[idxBuf][idxChan] = NULL;
	net[idxBuf][idxChan] = NULL;
	offset[idxBuf][idxChan] = 0;
	return JVX_NO_ERROR;
}

#ifndef JVX_COMPILE_SMALL
jvxErrorType
jvx_allocateDataLinkPipelineControl(jvxLinkDataDescriptor* theData
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
, const char* pipe_ref
#endif
)
{
	jvxSize i;

	// If we use the fields passed from outside, do nothing
	if (jvx_bitTest(theData->con_data.alloc_flags, 
		(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT))
	{
		return JVX_NO_ERROR;
	}

	if (theData->con_data.number_buffers)
	{
		// Allocate buffer reservation handle
		assert(theData->con_pipeline.reserve_buffer_pipeline_stage == NULL);
		JVX_DSP_SAFE_ALLOCATE_FIELD(theData->con_pipeline.reserve_buffer_pipeline_stage, pipeline_reserve, 
			theData->con_data.number_buffers);
		for (i = 0; i < theData->con_data.number_buffers; i++)
		{
			theData->con_pipeline.reserve_buffer_pipeline_stage[i].idProcess = JVX_SIZE_UNSELECTED;
			theData->con_pipeline.reserve_buffer_pipeline_stage[i].idStage_to_be_processed_by_stage = 0;
		}

		JVX_DSP_SAFE_ALLOCATE_OBJECT(theData->con_pipeline.idx_stage_ptr, jvxSize);
		*theData->con_pipeline.idx_stage_ptr = 0;

		// Create buffer pipeline lock handle
		theData->con_pipeline.do_lock = jvx_static_lock;
		theData->con_pipeline.do_try_lock = jvx_static_try_lock;
		theData->con_pipeline.do_unlock = jvx_static_unlock;
		JVX_MUTEX_HANDLE* ptrLock = NULL;
		JVX_SAFE_NEW_OBJ(ptrLock, JVX_MUTEX_HANDLE);
		JVX_INITIALIZE_MUTEX(*ptrLock);
		theData->con_pipeline.lock_hdl = ptrLock;
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
		theData->con_pipeline.debug_ptr = pipe_ref;
#endif
	}
	return JVX_NO_ERROR;
}

jvxErrorType
jvx_deallocateDataLinkPipelineControl(jvxLinkDataDescriptor* theData)
{
	// If we use the fields passed from outside, do nothing
	if (jvx_bitTest(theData->con_data.alloc_flags, 
		(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT))
	{
		return JVX_NO_ERROR;
	}
	if (theData->con_data.number_buffers)
	{
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
		theData->con_pipeline.debug_ptr = NULL;
#endif

		// Remove buffer pipeline lock handle
		theData->con_pipeline.do_lock = NULL;
		theData->con_pipeline.do_try_lock = NULL;
		theData->con_pipeline.do_unlock = NULL;
		JVX_MUTEX_HANDLE* ptrLock = (JVX_MUTEX_HANDLE*)theData->con_pipeline.lock_hdl;
		theData->con_pipeline.lock_hdl = NULL;
		JVX_TERMINATE_MUTEX(*ptrLock);
		JVX_SAFE_DELETE_OBJ(ptrLock);
		theData->con_pipeline.do_lock = NULL;
		theData->con_pipeline.do_try_lock = NULL;
		theData->con_pipeline.do_unlock = NULL;

		// Deallocate buffer reservation handle
		JVX_DSP_SAFE_DELETE_FIELD(theData->con_pipeline.reserve_buffer_pipeline_stage);
		theData->con_pipeline.reserve_buffer_pipeline_stage = NULL;

		JVX_DSP_SAFE_DELETE_OBJECT(theData->con_pipeline.idx_stage_ptr);
	}
	return JVX_NO_ERROR;
}
#endif

jvxErrorType
jvx_allocateDataLinkSync(jvxLinkDataDescriptor* theData)
{
	/* JVX_DATA_SYNC_FRAME_COUNT_UINT64 = 0x1,
	JVX_DATA_SYNC_FRAME_TIMESTAMP_TICK = 0x2,
	JVX_DATA_SYNC_DATA_COUNT_UINT64 = 0x4,
	JVX_DATA_SYNC_DATA_TIMESTAMP_TICK = 0x8,
	JVX_DATA_SYNC_DATA_TIMESTAMP_FLOAT
	*/
	if (theData->con_sync.type_timestamp & JVX_DATA_SYNC_FRAME_TIMESTAMP_TICK)
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(theData->con_sync.reserve_timestamp, jvxTick, theData->con_data.number_buffers);
		theData->con_sync.type_timestamp = JVX_DATA_SYNC_FRAME_TIMESTAMP_TICK;
	}
	else if (theData->con_sync.type_timestamp & JVX_DATA_SYNC_FRAME_COUNT_TICK)
	{
		JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(theData->con_sync.reserve_timestamp, jvxTick, theData->con_data.number_buffers);
		theData->con_sync.type_timestamp = JVX_DATA_SYNC_FRAME_COUNT_TICK;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
jvx_deallocateDataLinkSync(jvxLinkDataDescriptor* theData)
{
	/* JVX_DATA_SYNC_FRAME_COUNT_UINT64 = 0x1,
	JVX_DATA_SYNC_FRAME_TIMESTAMP_TICK = 0x2,
	JVX_DATA_SYNC_DATA_COUNT_UINT64 = 0x4,
	JVX_DATA_SYNC_DATA_TIMESTAMP_TICK = 0x8,
	JVX_DATA_SYNC_DATA_TIMESTAMP_FLOAT
	*/
	if (theData->con_sync.type_timestamp & JVX_DATA_SYNC_FRAME_TIMESTAMP_TICK)
	{
	    JVX_DSP_SAFE_DELETE_FIELD_TYPE(theData->con_sync.reserve_timestamp, jvxTick);
		theData->con_sync.type_timestamp = 0;
	}
	else if (theData->con_sync.type_timestamp & JVX_DATA_SYNC_FRAME_COUNT_TICK)
	{
	    JVX_DSP_SAFE_DELETE_FIELD_TYPE(theData->con_sync.reserve_timestamp, jvxTick);
		theData->con_sync.type_timestamp = 0;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
jvx_allocateDataLinkDescriptor(jvxLinkDataDescriptor* theData, jvxBool allocateFields)
{

	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i, j;

	assert(theData);

	// If the caller requests to use the specified buffers, do so!
	if (jvx_bitTest(theData->con_data.alloc_flags, 
		(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT))
	{
		return res;
	}

#ifdef JVX_COMPILE_SMALL

	// On very small devices, linked fields must use static buffers
	//return JVX_ERROR_UNSUPPORTED;
	assert(allocateFields == false);
#endif
	// =============================================================================================
	// =============================================================================================
	if (theData->con_data.number_buffers)
	{
		assert(theData->con_data.attached_buffer_single == NULL);
		JVX_DSP_SAFE_ALLOCATE_FIELD(theData->con_data.attached_buffer_single,
			jvxLinkDataAttached*,
			theData->con_data.number_buffers);

		assert(theData->con_data.attached_buffer_persist == NULL);
		JVX_DSP_SAFE_ALLOCATE_FIELD(theData->con_data.attached_buffer_persist,
			jvxLinkDataCombinedInformation*,
			theData->con_data.number_buffers);

		assert(theData->con_data.buffers == NULL);
		JVX_DSP_SAFE_ALLOCATE_FIELD(theData->con_data.buffers,
			jvxHandle**,
			theData->con_data.number_buffers);
		assert(theData->con_data.buffers);

		// =============================================================================================

		assert(theData->con_data.bExt.raw == NULL);
		JVX_DSP_SAFE_ALLOCATE_FIELD(theData->con_data.bExt.raw,
			jvxHandle**,
			theData->con_data.number_buffers);
		assert(theData->con_data.buffers);

		// =============================================================================================

		assert(theData->con_data.bExt.offset == NULL);
		JVX_DSP_SAFE_ALLOCATE_FIELD(theData->con_data.bExt.offset,
			jvxSize*,
			theData->con_data.number_buffers);
		assert(theData->con_data.buffers);

		// =============================================================================================
		// =============================================================================================

		for (i = 0; i < theData->con_data.number_buffers; i++)
		{
			theData->con_data.attached_buffer_single[i] = NULL;
			theData->con_data.attached_buffer_persist[i] = NULL;
			theData->con_data.buffers[i] = NULL;
			theData->con_data.bExt.raw[i] = NULL;
			if (theData->con_params.number_channels)
			{
				JVX_DSP_SAFE_ALLOCATE_FIELD(theData->con_data.buffers[i], jvxHandle*, theData->con_params.number_channels);
				assert(theData->con_data.buffers[i]);
				JVX_DSP_SAFE_ALLOCATE_FIELD(theData->con_data.bExt.raw[i], jvxHandle*, theData->con_params.number_channels);
				assert(theData->con_data.bExt.raw[i]);
				JVX_DSP_SAFE_ALLOCATE_FIELD(theData->con_data.bExt.offset[i], jvxSize, theData->con_params.number_channels);
				assert(theData->con_data.bExt.offset[i]);
				for (j = 0; j < (jvxSize)theData->con_params.number_channels; j++)
				{
					theData->con_data.bExt.raw[i][j] = NULL;
					theData->con_data.buffers[i][j] = NULL;
					theData->con_data.bExt.offset[i][j] = 0;
					theData->con_data.bExt.sz = 0;
					if (allocateFields)
					{
						jvxSize szElm = jvxDataFormat_size[theData->con_params.format];
						assert(szElm);
						jvx_allocateField_c_a(&theData->con_data.bExt.raw[i][j], theData->con_params.buffersize,
							szElm, (theData->con_data.bExt.prepend + theData->con_data.bExt.append + theData->con_data.bExt.align),
							&theData->con_data.bExt.sz, theData->con_data.bExt.f_alloc, theData->con_data.bExt.f_priv);

						assert(theData->con_data.bExt.raw[i][j]);
						jvxSize ptrI = (jvxSize)((jvxByte*)theData->con_data.bExt.raw[i][j]);
						jvxSize dec = 0;
						jvxSize inc = theData->con_data.bExt.prepend;
						if (theData->con_data.bExt.align)
						{
							// 0x12345 align 4: 0x12345 + 4 - 1
							inc += (theData->con_data.bExt.align - 1);

							// Here is my attempt: 
							// convert pointer to int, do the align and compute the pointer decrement
							// and finally apply the decrement
							dec = (ptrI + inc) % theData->con_data.bExt.align;
						}
						theData->con_data.bExt.offset[i][j] = inc - dec;
						theData->con_data.buffers[i][j] =
							((jvxByte*)theData->con_data.bExt.raw[i][j] + theData->con_data.bExt.offset[i][j]);
					}
				}
			}
		}
	}
	return(res);
}

jvxErrorType
jvx_deallocateDataLinkDescriptor(jvxLinkDataDescriptor* theData, jvxBool deallocateFields)
{
	jvxSize i, j;
	jvxErrorType res = JVX_NO_ERROR;

	assert(theData);

	// If the caller requests to use the specified buffers, do not deallocate buffers!
	if (jvx_bitTest(theData->con_data.alloc_flags, 
		(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT))
	{
		return res;
	}

#ifdef JVX_COMPILE_SMALL

	// On very small devices, linked fields must use static buffers
	//return JVX_ERROR_UNSUPPORTED;
	assert(deallocateFields == false);
#endif

	// !! Here, we do not check that the local dataprocessing environment is in right state !!
	if (theData->con_data.number_buffers)
	{
		for (i = 0; i < theData->con_data.number_buffers; i++)
		{
			if (theData->con_params.number_channels)
			{
				for (j = 0; j < (jvxSize)theData->con_params.number_channels; j++)
				{
					if (deallocateFields)
					{
						jvx_deallocateField_c_a(&theData->con_data.bExt.raw[i][j], 
							theData->con_data.bExt.f_dealloc, theData->con_data.bExt.f_priv);
						theData->con_data.buffers[i][j] = NULL;
						theData->con_data.bExt.offset[i][j] = 0;
					}
					else
					{
						assert(theData->con_data.buffers[i][j] == NULL);
					}
				}
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(theData->con_data.buffers[i], jvxHandle*);
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(theData->con_data.bExt.raw[i], jvxHandle*);
				JVX_DSP_SAFE_DELETE_FIELD_TYPE(theData->con_data.bExt.offset[i], jvxSize);
			}
		}

		JVX_DSP_SAFE_DELETE_FIELD_TYPE(theData->con_data.buffers, jvxHandle**);
		JVX_DSP_SAFE_DELETE_FIELD_TYPE(theData->con_data.bExt.raw, jvxHandle**);
		JVX_DSP_SAFE_DELETE_FIELD_TYPE(theData->con_data.bExt.offset, jvxSize*);
		JVX_DSP_SAFE_DELETE_FIELD_TYPE(theData->con_data.attached_buffer_single, jvxLinkDataAttached*);
		JVX_DSP_SAFE_DELETE_FIELD_TYPE(theData->con_data.attached_buffer_persist, jvxLinkDataCombinedInformation*);

	}
	return res;
}

jvxErrorType 
jvx_copyDataLinkDescriptorSync(jvxLinkDataDescriptor* copyFrom, jvxSize idx_from,
	jvxLinkDataDescriptor* copyTo, jvxSize idx_to)
{
	if (
		JVX_CHECK_SIZE_SELECTED(idx_from) &&
		JVX_CHECK_SIZE_SELECTED(idx_to))
	{
		if (copyFrom->con_sync.reserve_timestamp && copyTo->con_sync.reserve_timestamp)
		{
			if (copyFrom->con_sync.type_timestamp == copyTo->con_sync.type_timestamp)
			{
				switch (copyFrom->con_sync.type_timestamp)
				{
				case JVX_DATA_SYNC_FRAME_COUNT_TICK:
				case JVX_DATA_SYNC_FRAME_TIMESTAMP_TICK:
					((jvxTick*)copyTo->con_sync.reserve_timestamp)[idx_to] =
						((jvxTick*)copyFrom->con_sync.reserve_timestamp)[idx_from];
					break;
				}
			}
		}
	}
	return JVX_NO_ERROR;
}


jvxErrorType
jvx_copyDataLinkDescriptor(jvxLinkDataDescriptor* copyTo, jvxLinkDataDescriptor* copyFrom)
{
	copyTo->con_params.buffersize = copyFrom->con_params.buffersize;
	copyTo->con_params.format = copyFrom->con_params.format;
	copyTo->con_data.number_buffers = copyFrom->con_data.number_buffers;
	copyTo->con_params.number_channels = copyFrom->con_params.number_channels;
	copyTo->con_params.rate = copyFrom->con_params.rate;

	copyTo->con_params = copyFrom->con_params;

	copyTo->con_data.bExt.align = copyFrom->con_data.bExt.align;
	copyTo->con_data.bExt.append = copyFrom->con_data.bExt.append;
	copyTo->con_data.bExt.prepend = copyFrom->con_data.bExt.prepend;
	return(JVX_NO_ERROR);
}

jvxErrorType
jvx_copyDataLinkDescriptor_data(jvxLinkDataDescriptor* copyTo, jvxLinkDataDescriptor* copyFrom)
{
	copyTo->con_data.buffers = copyFrom->con_data.buffers;
	copyTo->con_data.bExt = copyFrom->con_data.bExt;
	copyTo->con_pipeline = copyFrom->con_pipeline;
	return(JVX_NO_ERROR);
}

jvxErrorType
jvx_ccopyDataLinkDescriptor(jvxLinkDataDescriptor* copyTo, jvxLinkDataDescriptor* copyFrom)
{
	copyTo->con_compat.buffersize = copyFrom->con_params.buffersize;
	copyTo->con_compat.format = copyFrom->con_params.format;
	copyTo->con_compat.number_buffers = copyFrom->con_data.number_buffers;
	copyTo->con_compat.number_channels = copyFrom->con_params.number_channels;
	copyTo->con_compat.rate = copyFrom->con_params.rate;

	copyTo->con_compat.ext.segmentation_x = copyFrom->con_params.segmentation_x;
	copyTo->con_compat.ext.segmentation_y = copyFrom->con_params.segmentation_y;
	copyTo->con_compat.ext.subformat = copyFrom->con_params.caps.format_group;
	copyTo->con_compat.ext.hints = copyFrom->con_params.hints;

	copyTo->con_compat.bExt.align = copyFrom->con_data.bExt.align;
	copyTo->con_compat.bExt.append = copyFrom->con_data.bExt.append;
	copyTo->con_compat.bExt.prepend = copyFrom->con_data.bExt.prepend;

	return(JVX_NO_ERROR);
}

jvxErrorType
jvx_cinitDataLinkDescriptor(jvxLinkDataDescriptor* descr)
{
	descr->con_compat.buffersize = 0;
	descr->con_compat.format = JVX_DATAFORMAT_NONE;
	descr->con_compat.number_buffers = 0;
	descr->con_compat.number_channels = 0;
	descr->con_compat.rate = 0;

	descr->con_compat.ext.hints = 0;
	//descr->con_compat.ext.segmentation_x = 0;
	//descr->con_compat.ext.segmentation_y = 0;
	//descr->con_compat.ext.subformat = JVX_DATAFORMAT_GROUP_NONE;

	descr->con_compat.bExt.align = 0;
	descr->con_compat.bExt.append = 0;
	descr->con_compat.bExt.prepend = 0;

	descr->con_compat.bExt.f_alloc = NULL;
	descr->con_compat.bExt.f_dealloc = NULL;
	descr->con_compat.bExt.f_priv = NULL;
			
	return(JVX_NO_ERROR);
}

void
jvx_neutralDataLinkDescriptor(jvxLinkDataDescriptor* theData, jvxBool sender)
{
	jvx_neutralDataLinkDescriptor_mem(theData, sender);

	if (sender)
	{
		theData->con_params.buffersize = 0;
		theData->con_params.format = JVX_DATAFORMAT_NONE;
		theData->con_data.buffers = NULL;
		theData->con_data.number_buffers = 0;
		theData->con_params.number_channels = 0;
		theData->con_params.rate = 0;
		theData->con_params.hints = 0;
		theData->con_params.segmentation_x = 0;
		theData->con_params.segmentation_y = 0;
		theData->con_params.caps.format_group = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;
		theData->con_data.alloc_flags = 
			(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_NONE;

	}
	else
	{

		theData->con_compat.buffersize = 0;
		theData->con_compat.format = JVX_DATAFORMAT_NONE;
		theData->con_compat.from_receiver_buffer_allocated_by_sender = NULL;
		theData->con_compat.number_buffers = 0;
		theData->con_compat.number_channels = 0;
		theData->con_compat.rate = 0;
		theData->con_compat.ext.hints = 0;
		//theData->con_compat.ext.segmentation_x = 0;
		//theData->con_compat.ext.segmentation_y = 0;
		//theData->con_compat.ext.subformat = JVX_DATAFORMAT_GROUP_AUDIO_PCM;
	}
}

void
jvx_neutralDataLinkDescriptor_sync(jvxLinkDataDescriptor_con_sync* con_sync)
{
	con_sync->reserve_timestamp = NULL;
	con_sync->type_timestamp = 0;
}

void
jvx_neutralDataLinkDescriptor_data(jvxLinkDataDescriptor_con_data* con_data)
{
	con_data->bExt.align = 0;
	con_data->bExt.prepend = 0;
	con_data->bExt.append = 0;
	con_data->bExt.flags = 0;
	con_data->bExt.offset = NULL;
	con_data->bExt.raw = NULL;
	con_data->bExt.sz = 0;
	con_data->bExt.f_alloc = NULL;
	con_data->bExt.f_dealloc = NULL;
	con_data->bExt.f_priv = NULL;
	con_data->buffers = NULL;
}

void
jvx_neutralDataLinkDescriptor_mem(jvxLinkDataDescriptor* theData, jvxBool sender)
{
	if (sender)
	{
		theData->con_data.bExt.align = 0;
		theData->con_data.bExt.prepend = 0;
		theData->con_data.bExt.append = 0;
		theData->con_data.bExt.flags = 0;
		theData->con_data.bExt.offset = NULL;
		theData->con_data.bExt.raw = NULL;
		theData->con_data.bExt.sz = 0;
		theData->con_data.bExt.f_alloc = NULL;
		theData->con_data.bExt.f_dealloc = NULL;
		theData->con_data.bExt.f_priv = NULL;
		theData->con_data.buffers = NULL;
	}
	else
	{
		theData->con_compat.bExt.align = 0;
		theData->con_compat.bExt.prepend = 0;
		theData->con_compat.bExt.append = 0;
		theData->con_compat.bExt.flags = 0;
		theData->con_compat.bExt.offset = NULL;
		theData->con_compat.bExt.raw = NULL;
		theData->con_compat.bExt.sz = 0;
		theData->con_compat.bExt.f_alloc = NULL;
		theData->con_compat.bExt.f_dealloc = NULL;
		theData->con_compat.bExt.f_priv = NULL;
		theData->con_compat.from_receiver_buffer_allocated_by_sender = NULL;
		theData->con_compat.user_hints = NULL;

	}
}

void
jvx_neutralDataLinkDescriptor_pipeline(jvxLinkDataDescriptor_con_pipeline* thePipeline)
{
#ifndef JVX_COMPILE_SMALL
	thePipeline->do_lock = NULL;
	thePipeline->do_unlock = NULL;
	thePipeline->do_try_lock = NULL;
	thePipeline->lock_hdl = NULL;
	thePipeline->reserve_buffer_pipeline_stage = NULL;
#endif
	thePipeline->num_additional_pipleline_stages = 0;
	thePipeline->idx_stage_ptr = NULL;
}

#define JVX_CHECK_PARAM_BUFFERSIZE_SHIFT 0
#define JVX_CHECK_PARAM_SAMPLERATE_SHIFT 1
#define JVX_CHECK_PARAM_FORMAT_SHIFT 2
#define JVX_CHECK_PARAM_NUM_CHANNELS_SHIFT 3
#define JVX_CHECK_PARAM_SEGMENTATION_X_SHIFT 4
#define JVX_CHECK_PARAM_SEGMENTATION_Y_SHIFT 5
#define JVX_CHECK_PARAM_SUBFORMAT_SHIFT 6

jvxBool
jvx_check_in_out_params_match_test(jvxLinkDataDescriptor* cp_this, jvxLinkDataDescriptor* cp_that, jvxCBitField bfld)
{
	jvxBool retVal = true;
	if (jvx_bitTest(bfld, JVX_CHECK_PARAM_BUFFERSIZE_SHIFT))
	{
		retVal = retVal && (cp_this->con_params.buffersize == cp_that->con_params.buffersize);
	}
	if (jvx_bitTest(bfld, JVX_CHECK_PARAM_SAMPLERATE_SHIFT))
	{
		retVal = retVal && (cp_this->con_params.rate == cp_that->con_params.rate);
	}
	if (jvx_bitTest(bfld, JVX_CHECK_PARAM_FORMAT_SHIFT))
	{
		retVal = retVal && (cp_this->con_params.format == cp_that->con_params.format);
	}
	if (jvx_bitTest(bfld, JVX_CHECK_PARAM_NUM_CHANNELS_SHIFT))
	{
		retVal = retVal && (cp_this->con_params.number_channels == cp_that->con_params.number_channels);
	}
	if (jvx_bitTest(bfld, JVX_CHECK_PARAM_SEGMENTATION_X_SHIFT))
	{
		retVal = retVal && (cp_this->con_params.segmentation_x == cp_that->con_params.segmentation_x);
	}
	if (jvx_bitTest(bfld, JVX_CHECK_PARAM_SEGMENTATION_Y_SHIFT))
	{
		retVal = retVal && (cp_this->con_params.segmentation_y == cp_that->con_params.segmentation_y);
	}
	if (jvx_bitTest(bfld, JVX_CHECK_PARAM_SUBFORMAT_SHIFT))
	{
		retVal = retVal && (cp_this->con_params.caps.format_group == cp_that->con_params.caps.format_group);
	}
	return retVal;
}

jvxBool jvx_check_in_out_params_match_test_err(jvxLinkDataDescriptor* cp_this, jvxLinkDataDescriptor* cp_that, jvxCBitField bfld, std::string& errStr)
{
	jvxBool retVal = true;
	errStr.clear();

	if (jvx_bitTest(bfld, JVX_CHECK_PARAM_BUFFERSIZE_SHIFT))
	{
		if (cp_this->con_params.buffersize != cp_that->con_params.buffersize)
		{
			if (!errStr.empty())
			{
				errStr += "; ";
			}
			errStr += "Buffersize mismatch: Parameter value <" + jvx_size2String(cp_this->con_params.buffersize) + " vs " +
				jvx_size2String(cp_that->con_params.buffersize);
			retVal = false;
		}
	}
	if (jvx_bitTest(bfld, JVX_CHECK_PARAM_SAMPLERATE_SHIFT))
	{
		if (cp_this->con_params.rate != cp_that->con_params.rate)
		{
			if (!errStr.empty())
			{
				errStr += "; ";
			}
			errStr += "Samplerate mismatch: Parameter value <" + jvx_size2String(cp_this->con_params.rate) + " vs " +
				jvx_size2String(cp_that->con_params.rate);
			retVal = false;
		}
	}
	if (jvx_bitTest(bfld, JVX_CHECK_PARAM_FORMAT_SHIFT))
	{
		if (cp_this->con_params.format != cp_that->con_params.format)
		{
			if (!errStr.empty())
			{
				errStr += "; ";
			}
			errStr += "Format mismatch: Parameter value <";
			errStr += jvxDataFormat_txt(cp_this->con_params.format);
			errStr += " vs ";
			errStr += jvxDataFormat_txt(cp_that->con_params.format);
			retVal = false;
		}
	}
	if (jvx_bitTest(bfld, JVX_CHECK_PARAM_NUM_CHANNELS_SHIFT))
	{
		if (cp_this->con_params.number_channels != cp_that->con_params.number_channels)
		{
			if (!errStr.empty())
			{
				errStr += "; ";
			}
			errStr += "Channelnumber mismatch: Parameter value <" + jvx_size2String(cp_this->con_params.number_channels) + " vs " +
				jvx_size2String(cp_that->con_params.number_channels);
			retVal = false;
		}
	}
	if (jvx_bitTest(bfld, JVX_CHECK_PARAM_SEGMENTATION_X_SHIFT))
	{
		if (cp_this->con_params.segmentation_x != cp_that->con_params.segmentation_x)
		{
			if (!errStr.empty())
			{
				errStr += "; ";
			}
			errStr += "Segmentation X mismatch: Parameter value <" + jvx_size2String(cp_this->con_params.segmentation_x) + " vs " +
				jvx_size2String(cp_that->con_params.segmentation_x);
			retVal = false;
		}
	}
	if (jvx_bitTest(bfld, JVX_CHECK_PARAM_SEGMENTATION_Y_SHIFT))
	{
		if (cp_this->con_params.segmentation_y != cp_that->con_params.segmentation_y)
		{
			if (!errStr.empty())
			{
				errStr += "; ";
			}
			errStr += "Segmentation Y mismatch: Parameter value <" + jvx_size2String(cp_this->con_params.segmentation_y) + " vs " +
				jvx_size2String(cp_that->con_params.segmentation_y);
			retVal = false;
		}
	}
	if (jvx_bitTest(bfld, JVX_CHECK_PARAM_SUBFORMAT_SHIFT))
	{
		if (cp_this->con_params.caps.format_group != cp_that->con_params.caps.format_group)
		{
			if (!errStr.empty())
			{
				errStr += "; ";
			}
			errStr += "Format mismatch: Parameter value <";
			errStr += jvxDataFormatGroup_txt(cp_this->con_params.caps.format_group);
			errStr += " vs ";
			errStr += jvxDataFormatGroup_txt(cp_that->con_params.caps.format_group);
			retVal = false;
		}
	}
	return retVal;
}
