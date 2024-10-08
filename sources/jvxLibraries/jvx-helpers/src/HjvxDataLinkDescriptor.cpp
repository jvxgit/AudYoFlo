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
jvx_allocateDataLinkDescriptorRouteChannels(
		jvxLinkDataDescriptor* theDataTo, jvxLinkDataDescriptor* theDataFrom, 
	jvxSize numChannelsCopy, jvxHandle**** bufToStore, 
	jvxSize* ptrEntriesRoutes, jvxSize numEntriesRoutes, jvxBool routeOutputToInput)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i, j;

	assert(theDataTo);

	// If the caller requests to use the specified buffers, do so!
	if (jvx_bitTest(theDataTo->con_data.alloc_flags,
		(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT))
	{
		return res;
	}

#ifdef JVX_COMPILE_SMALL

	// On very small devices, linked fields must use static buffers
	//return JVX_ERROR_UNSUPPORTED;
	assert(allocateFields == false);
#endif

	assert(theDataTo->con_data.number_buffers == theDataFrom->con_data.number_buffers);
	assert(numChannelsCopy <= theDataTo->con_params.number_channels );
	assert(numChannelsCopy <= theDataFrom->con_params.number_channels );

	// =============================================================================================
	// =============================================================================================
	if (theDataTo->con_data.number_buffers)
	{
		if (bufToStore) *bufToStore = theDataTo->con_data.buffers;
		JVX_DSP_SAFE_ALLOCATE_FIELD(theDataTo->con_data.buffers,
			jvxHandle**,
			theDataTo->con_data.number_buffers);
		assert(theDataTo->con_data.buffers);

		// =============================================================================================
		// =============================================================================================

		for (i = 0; i < theDataTo->con_data.number_buffers; i++)
		{
			theDataTo->con_data.buffers[i] = NULL;

			switch (theDataTo->con_params.format_group)
			{
			case JVX_DATAFORMAT_GROUP_FFMPEG_PACKET_FWD:
			case JVX_DATAFORMAT_GROUP_FFMPEG_FRAME_FWD:

				// The buffers are copied "forward", that is the pointer will be forwarded from component to component in theData->con_data.buffers[i]
				// Do not allocate anything here!
				break;
			default:
				if (theDataTo->con_params.number_channels)
				{
					JVX_DSP_SAFE_ALLOCATE_FIELD(theDataTo->con_data.buffers[i], jvxHandle*, theDataTo->con_params.number_channels);
					assert(theDataTo->con_data.buffers[i]);

					for (j = 0; j < (jvxSize)theDataTo->con_params.number_channels; j++)
					{
						theDataTo->con_data.buffers[i][j] = NULL;
					}

					// Here we actually copy the channels
					for (j = 0; j < (jvxSize)theDataTo->con_params.number_channels; j++)
					{
						if (j < numChannelsCopy)
						{
							jvxSize writeTo = j;
							jvxSize writeFrom = j;

							if (j < numEntriesRoutes)
							{
								if (routeOutputToInput)
								{
									writeTo = ptrEntriesRoutes[j];
								}
								else
								{
									writeFrom = ptrEntriesRoutes[j];
								}
							}

							writeTo = JVX_MIN(writeTo, theDataTo->con_params.number_channels - 1);
							writeFrom = JVX_MIN(writeFrom, theDataFrom->con_params.number_channels - 1);

							theDataTo->con_data.buffers[i][writeTo] = theDataFrom->con_data.buffers[i][writeFrom];
						}
					}
				}
			}
		}
	}
	return res;
}

jvxErrorType
jvx_deallocateDataLinkDescriptorRouteChannels(jvxLinkDataDescriptor* theData, jvxHandle*** bufFromStorage)
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
			// The buffers are copied "forward", that is the pointer will be forwarded from component to component in theData->con_data.buffers[i]
			switch (theData->con_params.format_group)
			{
			case JVX_DATAFORMAT_GROUP_FFMPEG_PACKET_FWD:
			case JVX_DATAFORMAT_GROUP_FFMPEG_FRAME_FWD:

				// The buffers are copied "forward", that is the pointer will be forwarded from component to component in theData->con_data.buffers[i]
				// Do not allocate anything here!
				break;
			default:
				if (theData->con_params.number_channels)
				{
					for (j = 0; j < (jvxSize)theData->con_params.number_channels; j++)
					{
						theData->con_data.buffers[i][j] = nullptr;
					}
					JVX_DSP_SAFE_DELETE_FIELD_TYPE(theData->con_data.buffers[i], jvxHandle*);
				}
			}
		}

		JVX_DSP_SAFE_DELETE_FIELD_TYPE(theData->con_data.buffers, jvxHandle**);
	}
	theData->con_data.buffers = bufFromStorage;
	return res;
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
			jvxLinkDataAttachedChain*,
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

		if (
			(jvx_bitTest(theData->con_data.alloc_flags,
			(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_EXPECT_FHEIGHT_INFO_SHIFT)) || 
			(jvx_bitTest(theData->con_data.alloc_flags,
				(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_ALLOW_FHEIGHT_INFO_SHIFT)))
		{
			assert(theData->con_data.fHeights == NULL);
			JVX_DSP_SAFE_ALLOCATE_FIELD(theData->con_data.fHeights,
				jvxLinkDataDescriptor_segmentation,
				theData->con_data.number_buffers);
			assert(theData->con_data.fHeights);
		}

		// =============================================================================================
		// =============================================================================================

		for (i = 0; i < theData->con_data.number_buffers; i++)
		{
			theData->con_data.attached_buffer_single[i] = NULL;
			theData->con_data.attached_buffer_persist[i] = NULL;
			theData->con_data.buffers[i] = NULL;
			theData->con_data.bExt.raw[i] = NULL;

			switch (theData->con_params.format_group)
			{
			case JVX_DATAFORMAT_GROUP_FFMPEG_PACKET_FWD:
			case JVX_DATAFORMAT_GROUP_FFMPEG_FRAME_FWD:

				// The buffers are copied "forward", that is the pointer will be forwarded from component to component in theData->con_data.buffers[i]
				// Do not allocate anything here!
				break;
			default:
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
				break;
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
			// The buffers are copied "forward", that is the pointer will be forwarded from component to component in theData->con_data.buffers[i]
			switch (theData->con_params.format_group)
			{
			case JVX_DATAFORMAT_GROUP_FFMPEG_PACKET_FWD:
			case JVX_DATAFORMAT_GROUP_FFMPEG_FRAME_FWD:

				// The buffers are copied "forward", that is the pointer will be forwarded from component to component in theData->con_data.buffers[i]
				// Do not allocate anything here!
				break;
			default:
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
		}

		JVX_DSP_SAFE_DELETE_FIELD_TYPE(theData->con_data.fHeights, jvxLinkDataDescriptor_segmentation);			
		JVX_DSP_SAFE_DELETE_FIELD_TYPE(theData->con_data.buffers, jvxHandle**);
		JVX_DSP_SAFE_DELETE_FIELD_TYPE(theData->con_data.bExt.raw, jvxHandle**);
		JVX_DSP_SAFE_DELETE_FIELD_TYPE(theData->con_data.bExt.offset, jvxSize*);
		JVX_DSP_SAFE_DELETE_FIELD_TYPE(theData->con_data.attached_buffer_single, jvxLinkDataAttachedChain*);
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

	copyTo->con_compat.ext.segmentation_x = copyFrom->con_params.segmentation.x;
	copyTo->con_compat.ext.segmentation_y = copyFrom->con_params.segmentation.y;
	copyTo->con_compat.ext.subformat = copyFrom->con_params.format_group;
	copyTo->con_compat.ext.additional_flags = copyFrom->con_params.additional_flags;

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

	descr->con_compat.ext.additional_flags = 0;
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
		theData->con_params.additional_flags = 0;
		theData->con_params.segmentation.x = 0;
		theData->con_params.segmentation.y = 0;
		theData->con_params.format_group = JVX_DATAFORMAT_GROUP_AUDIO_PCM_DEINTERLEAVED;
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
		theData->con_compat.ext.additional_flags = 0;
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
	con_data->bExt.offset = nullptr;
	con_data->bExt.raw = nullptr;
	con_data->bExt.sz = 0;
	con_data->bExt.f_alloc = nullptr;
	con_data->bExt.f_dealloc = nullptr;
	con_data->bExt.f_priv = nullptr;
	con_data->buffers = nullptr;
	con_data->attached_buffer_single = nullptr;
	con_data->attached_buffer_persist = nullptr;
	con_data->fHeights = nullptr;

	con_data->alloc_flags = 0;
	con_data->number_buffers = 0;
	con_data->user_hints = nullptr;
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
		retVal = retVal && (cp_this->con_params.segmentation.x == cp_that->con_params.segmentation.x);
	}
	if (jvx_bitTest(bfld, JVX_CHECK_PARAM_SEGMENTATION_Y_SHIFT))
	{
		retVal = retVal && (cp_this->con_params.segmentation.y == cp_that->con_params.segmentation.y);
	}
	if (jvx_bitTest(bfld, JVX_CHECK_PARAM_DATAFLOW_SHIFT))
	{
		retVal = retVal && (cp_this->con_params.data_flow == cp_that->con_params.data_flow);
	}
	if (jvx_bitTest(bfld, JVX_CHECK_PARAM_SUBFORMAT_SHIFT))
	{
		retVal = retVal && (cp_this->con_params.format_group == cp_that->con_params.format_group);
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
		if (cp_this->con_params.segmentation.x != cp_that->con_params.segmentation.x)
		{
			if (!errStr.empty())
			{
				errStr += "; ";
			}
			errStr += "Segmentation X mismatch: Parameter value <" + jvx_size2String(cp_this->con_params.segmentation.x) + " vs " +
				jvx_size2String(cp_that->con_params.segmentation.x);
			retVal = false;
		}
	}
	if (jvx_bitTest(bfld, JVX_CHECK_PARAM_SEGMENTATION_Y_SHIFT))
	{
		if (cp_this->con_params.segmentation.y != cp_that->con_params.segmentation.y)
		{
			if (!errStr.empty())
			{
				errStr += "; ";
			}
			errStr += "Segmentation Y mismatch: Parameter value <" + jvx_size2String(cp_this->con_params.segmentation.y) + " vs " +
				jvx_size2String(cp_that->con_params.segmentation.y);
			retVal = false;
		}
	}
	if (jvx_bitTest(bfld, JVX_CHECK_PARAM_SUBFORMAT_SHIFT))
	{
		if (cp_this->con_params.format_group != cp_that->con_params.format_group)
		{
			if (!errStr.empty())
			{
				errStr += "; ";
			}
			errStr += "Format mismatch: Parameter value <";
			errStr += jvxDataFormatGroup_txt(cp_this->con_params.format_group);
			errStr += " vs ";
			errStr += jvxDataFormatGroup_txt(cp_that->con_params.format_group);
			retVal = false;
		}
	}
	return retVal;
}

//! Allocate a list of "lost buffer" objects to be forwarded through the processing chain
void
jvxLinkDataAttachedLostFrames_allocateRuntime(
	std::list<jvxLinkDataAttached_oneFrameLostElement>& lst_cds,
	jvxHandle* prv_ptr,
	jvx_release_attached cb)
{
	jvxLinkDataAttached_oneFrameLostElement theElm;
	theElm.ptr = NULL;
	JVX_DSP_SAFE_ALLOCATE_OBJECT(theElm.ptr, jvxLinkDataAttachedLostFrames);
	theElm.ptr->priv = prv_ptr;
	theElm.ptr->cb_release = cb;
	theElm.inUse = false;
	theElm.ptr->numLost = 0;
	lst_cds.push_back(theElm);
}

//! Deallocate a list of "lost buffer" objects to be forwarded through the processing chain
void
jvxLinkDataAttachedLostFrames_deallocateRuntime(
	std::list<jvxLinkDataAttached_oneFrameLostElement>& lst_cds)
{
	auto elml = lst_cds.begin();
	for (; elml != lst_cds.end(); elml++)
	{
		JVX_DSP_SAFE_DELETE_OBJECT(elml->ptr);
	}
	lst_cds.clear();
}

//! Browse through the list of objects and return the first one that is available
jvxBool
jvxLinkDataAttachedLostFrames_updatePrepare(
	std::list<jvxLinkDataAttached_oneFrameLostElement>& lst_cds,
	jvxSize newCnt,
	jvxLinkDataAttachedLostFrames*& ptrOut)
{
	jvxBool elmentAvail = false;
	ptrOut = NULL;
	auto elm = lst_cds.begin();
	for (; elm != lst_cds.end(); elm++)
	{
		if (elm->inUse == false)
		{
			elm->inUse = true;
			elm->ptr->numLost = newCnt;
			ptrOut = elm->ptr;
			elmentAvail = true;
			break;
		}
	}
	return elmentAvail;
}

//! Browse through the list and release the provided element
jvxBool
jvxLinkDataAttachedLostFrames_updateComplete(
	std::list<jvxLinkDataAttached_oneFrameLostElement>& lst_cds,
	jvxLinkDataAttachedLostFrames* elm)
{
	jvxBool elmentExists = false;
	auto elml = lst_cds.begin();
	for (; elml != lst_cds.end(); elml++)
	{
		if (elm == (jvxLinkDataAttachedLostFrames*)elml->ptr)
		{
			elml->inUse = false;
			elmentExists = true;
			break;
		}
	}
	assert(elmentExists);
	return elmentExists;
}

// =================================================================================================

void
jvx_presetMasterOnPrepare(jvxLinkDataDescriptor& datOut)
{
	// Prepare processing parameters
	datOut.con_data.number_buffers = JVX_MAX(datOut.con_data.number_buffers, 1);
	jvx_bitZSet(datOut.con_data.alloc_flags, (int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_IS_ZEROCOPY_CHAIN_SHIFT);
	jvx_bitSet(datOut.con_data.alloc_flags, (int)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_EXPECT_FHEIGHT_INFO_SHIFT);
}

void
jvx_constrainIconOnPrepare(jvxLinkDataDescriptor* datIn, jvxBool clearFlagsBuffer, jvxSize num_additional_pipleline_stages, jvxSize num_min_buffers_in, jvxLinkDataDescriptor* datOut)
{
	// The number of pipeline stages may be increased from element to element
	datIn->con_pipeline.num_additional_pipleline_stages = JVX_MAX(
		datIn->con_pipeline.num_additional_pipleline_stages,
		num_additional_pipleline_stages);

	// The number of buffers is always lower bounded by the add pipeline stages
	datIn->con_data.number_buffers = JVX_MAX(
		datIn->con_data.number_buffers,
		1 + datIn->con_pipeline.num_additional_pipleline_stages);

	// We might specify another additional lower limit for the buffers
	datIn->con_data.number_buffers = JVX_MAX(
		datIn->con_data.number_buffers,
		num_min_buffers_in);

	if (datOut)
	{
		// Set the number of buffers as desired
		datOut->con_data.number_buffers = datIn->con_data.number_buffers;
		datOut->con_data.alloc_flags = datIn->con_data.alloc_flags;
	}

	if (clearFlagsBuffer)
	{
		jvx_bitClear(datIn->con_data.alloc_flags,
			(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_NO_ZEROCOPY_SHIFT);
		jvx_bitClear(datIn->con_data.alloc_flags,
			(jvxSize)jvxDataLinkDescriptorAllocFlags::JVX_LINKDATA_ALLOCATION_FLAGS_USE_PASSED_SHIFT);
	}
}

// =================================================================================================

jvxErrorType 
jvx_shift_buffer_pipeline_idx_on_start(
	jvxLinkDataDescriptor* theData, jvxSize runtmeId,
	jvxSize pipeline_offset, jvxSize* idx_stage,
	jvxSize tobeAccessedByStage ,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr )
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	jvxSize pipe_stage_in_use = 0;

#ifndef JVX_COMPILE_SMALL
	if (theData->con_pipeline.lock_hdl && theData->con_pipeline.do_unlock)
	{
		theData->con_pipeline.do_lock(theData->con_pipeline.lock_hdl);

		if (clbk)
		{
			res = clbk(&pipe_stage_in_use, priv_ptr);
			if (res == JVX_NO_ERROR)
			{
				if (JVX_CHECK_SIZE_UNSELECTED(theData->con_pipeline.reserve_buffer_pipeline_stage[
					pipe_stage_in_use].idProcess))
				{
#ifdef JVX_RESERVE_PIPELINE_VERBOSE
					std::cout << "<<< " << __FUNCTION__ << ": Reserving pipeline index " << pipe_stage_in_use << std::flush;
					if (theData->con_pipeline.debug_ptr)
					{
						std::cout << ", hint = " << theData->con_pipeline.debug_ptr << std::flush;
					}
					std::cout << std::endl;
#endif
					theData->con_pipeline.reserve_buffer_pipeline_stage[pipe_stage_in_use].idProcess = runtmeId;
					// assert(theData->con_pipeline.reserve_buffer_pipeline_stage[pipe_stage_in_use].idStage_processed_in_stage == tobeAccessedByStage);
					res = JVX_NO_ERROR;
				}
				else
				{
#ifdef JVX_RESERVE_PIPELINE_VERBOSE
					std::cout << "<<< " << __FUNCTION__ << ": Pipeline index " << pipe_stage_in_use << " is already in use." << std::endl;
					if (theData->con_pipeline.debug_ptr)
					{
						std::cout << ", hint = " << theData->con_pipeline.debug_ptr << std::endl;
					}
					std::cout << std::endl;
#endif
					res = JVX_ERROR_ALREADY_IN_USE;
				}
			}
		}
		else
		{
			if (
				(idx_stage) && (JVX_CHECK_SIZE_SELECTED(*idx_stage)))
			{
				pipe_stage_in_use = *idx_stage;
			}
			else
			{
				pipe_stage_in_use = (*theData->con_pipeline.idx_stage_ptr +
					(theData->con_data.number_buffers - pipeline_offset) %
					(theData->con_data.number_buffers));
			}

			if (JVX_CHECK_SIZE_UNSELECTED(theData->con_pipeline.reserve_buffer_pipeline_stage[
				pipe_stage_in_use].idProcess))
			{
#ifdef JVX_RESERVE_PIPELINE_VERBOSE
				std::cout << "<<< " << __FUNCTION__ << ": Reserving pipeline index " << pipe_stage_in_use << std::flush;
				if (theData->con_pipeline.debug_ptr)
				{
					std::cout << ", hint = " << theData->con_pipeline.debug_ptr << std::flush;
				}
				std::cout << std::endl;

#endif
				theData->con_pipeline.reserve_buffer_pipeline_stage[pipe_stage_in_use].idProcess = runtmeId;
				// assert(theData->con_pipeline.reserve_buffer_pipeline_stage[pipe_stage_in_use].idStage_processed_in_stage == tobeAccessedByStage);
				res = JVX_NO_ERROR;
			}
			else
			{
#ifdef JVX_RESERVE_PIPELINE_VERBOSE
				std::cout << "<<< " << __FUNCTION__ << ": Pipeline index " << pipe_stage_in_use << " is already in use" << std::flush;
				if (theData->con_pipeline.debug_ptr)
				{
					std::cout << ", hint = " << theData->con_pipeline.debug_ptr << std::flush;
				}
				std::cout << std::endl;
#endif
				res = JVX_ERROR_ALREADY_IN_USE;
			}
		}
		theData->con_pipeline.do_unlock(theData->con_pipeline.lock_hdl);

		if (idx_stage)
		{
			*idx_stage = pipe_stage_in_use;
		}
	}
#endif
	return res;
};

jvxErrorType 
jvx_shift_buffer_pipeline_idx_on_stop(jvxLinkDataDescriptor* theData,
	jvxSize idx_stage, jvxBool shift_fwd, jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk, jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_ERROR_NOT_READY;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSize report_idx = JVX_SIZE_UNSELECTED;
#ifndef JVX_COMPILE_SMALL

	if (theData->con_pipeline.lock_hdl && theData->con_pipeline.do_unlock)
	{
		theData->con_pipeline.do_lock(theData->con_pipeline.lock_hdl);

		if (JVX_CHECK_SIZE_SELECTED(idx_stage))
		{
#ifdef JVX_RESERVE_PIPELINE_VERBOSE
			std::cout << "<<< " << __FUNCTION__ << ": Releasing pipeline index " << idx_stage << std::flush;
			if (theData->con_pipeline.debug_ptr)
			{
				std::cout << ", hint = " << theData->con_pipeline.debug_ptr << std::flush;
			}
			std::cout << std::endl;
#endif
			report_idx = idx_stage;
			theData->con_pipeline.reserve_buffer_pipeline_stage[idx_stage].idProcess = JVX_SIZE_UNSELECTED;

			// Only used if pipeline is addressed directly

		}
		else
		{
			report_idx = *theData->con_pipeline.idx_stage_ptr;
#ifdef JVX_RESERVE_PIPELINE_VERBOSE
			std::cout << "<<< " << __FUNCTION__ << ": Releasing pipeline index " << report_idx << std::flush;
			if (theData->con_pipeline.debug_ptr)
			{
				std::cout << ", hint = " << theData->con_pipeline.debug_ptr << std::flush;
			}
			std::cout << std::endl;
#endif
			theData->con_pipeline.reserve_buffer_pipeline_stage[
				*theData->con_pipeline.idx_stage_ptr].idProcess = JVX_SIZE_UNSELECTED;
			theData->con_pipeline.reserve_buffer_pipeline_stage[
				*theData->con_pipeline.idx_stage_ptr].idStage_to_be_processed_by_stage = tobeAccessedByStage;

			// Pipeline progress only for main access (current time)
			if (shift_fwd)
			{
				*theData->con_pipeline.idx_stage_ptr =
					(*theData->con_pipeline.idx_stage_ptr + 1) %
					theData->con_data.number_buffers;
			}
		}

		// Report the stage idx which has just been released
		if (clbk)
		{
			resL = clbk(report_idx, priv_ptr);
			assert(resL == JVX_NO_ERROR);
		}

		theData->con_pipeline.do_unlock(theData->con_pipeline.lock_hdl);
		return JVX_NO_ERROR;
	}
#else
	theData->con_pipeline.idx_stage =
		(theData->con_pipeline.idx_stage + 1) %
		theData->con_data.number_buffers;
	res = JVX_NO_ERROR;
#endif
	return res;
};

// =================================================================================================

jvxErrorType  
jvx_allocate_pipeline_and_buffers_prepare_to(jvxLinkDataDescriptor* theData
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
	, const char* pipe_ref
#endif
)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = jvx_allocateDataLinkDescriptor(theData, true);
	assert(res == JVX_NO_ERROR);

#ifndef JVX_COMPILE_SMALL
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
	res = jvx_allocateDataLinkPipelineControl(theData, pipe_ref);
#else
	res = jvx_allocateDataLinkPipelineControl(theData);
#endif
	assert(res == JVX_NO_ERROR);
#endif
	JVX_DSP_SAFE_ALLOCATE_OBJECT(theData->con_pipeline.idx_stage_ptr, jvxSize);
	*theData->con_pipeline.idx_stage_ptr = 0;

	res = jvx_allocateDataLinkSync(theData);
	assert(res == JVX_NO_ERROR);

	return res;
};

jvxErrorType 
JVX_CALLINGCONVENTION jvx_allocate_pipeline_and_buffers_prepare_to_zerocopy(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i, j;

	assert(theData_out->con_data.number_buffers >= theData_in->con_data.number_buffers);
	assert(theData_out->con_params.number_channels >= theData_in->con_params.number_channels);
	assert(theData_out->con_params.buffersize >= theData_in->con_params.buffersize);
	assert(theData_out->con_params.format >= theData_in->con_params.format);

	theData_in->con_data.number_buffers = theData_out->con_data.number_buffers;

	res = jvx_allocateDataLinkDescriptor(theData_in, false);
	assert(res == JVX_NO_ERROR);

	// If there are output buffers, set shortcut references
	if (theData_out->con_data.buffers)
	{
		for (i = 0; i < theData_in->con_data.number_buffers; i++)
		{
			for (j = 0; j < theData_in->con_params.number_channels; j++)
			{
				theData_in->con_data.buffers[i][j] =
					theData_out->con_data.buffers[i][j];
			}
		}
	}
	theData_in->con_pipeline = theData_out->con_pipeline;

	theData_in->con_sync = theData_out->con_sync;

	return res;
};

jvxErrorType 
jvx_deallocate_pipeline_and_buffers_postprocess_to(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_NO_ERROR;

	res = jvx_deallocateDataLinkSync(theData);
	assert(res == JVX_NO_ERROR);

	JVX_DSP_SAFE_DELETE_OBJECT(theData->con_pipeline.idx_stage_ptr);

#ifndef JVX_COMPILE_SMALL
	res = jvx_deallocateDataLinkPipelineControl(theData);
	assert(res == JVX_NO_ERROR);
#endif
	res = jvx_deallocateDataLinkDescriptor(theData, true);
	assert(res == JVX_NO_ERROR);

	return res;
};

jvxErrorType 
jvx_deallocate_pipeline_and_buffers_postprocess_to_zerocopy(jvxLinkDataDescriptor* theData)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i, j;

	theData->con_sync.reserve_timestamp = NULL;
	theData->con_sync.type_timestamp = 0;

#ifndef JVX_COMPILE_SMALL
	theData->con_pipeline.do_lock = NULL;
	theData->con_pipeline.do_unlock = NULL;
	theData->con_pipeline.do_try_lock = NULL;
	theData->con_pipeline.lock_hdl = NULL;
	theData->con_pipeline.reserve_buffer_pipeline_stage = NULL;
#endif
	theData->con_pipeline.num_additional_pipleline_stages = 0;

	for (i = 0; i < theData->con_data.number_buffers; i++)
	{
		for (j = 0; j < theData->con_params.number_channels; j++)
		{
			theData->con_data.buffers[i][j] = NULL;
		}
	}
	res = jvx_deallocateDataLinkDescriptor(theData, false);
	assert(res == JVX_NO_ERROR);

	return res;
};

jvxLinkDataAttachedChain*
jvx_attached_push_front(jvxLinkDataAttachedChain* old_first, jvxLinkDataAttachedChain* new_first)
{
	jvxLinkDataAttachedChain* retVal = new_first;
	new_first->next = old_first;
	return retVal;
}

jvxLinkDataAttachedChain*
jvx_attached_pop_front(jvxLinkDataAttachedChain* old_first, jvxLinkDataAttachedChain** removed_first)
{
	jvxLinkDataAttachedChain* retVal = old_first->next;
	old_first->next = NULL;
	if (removed_first)
	{
		*removed_first = old_first;
	}
	return retVal;
}

jvxErrorType jvx_check_valid(jvxLinkDataDescriptor_con_params& params, std::string& errReason)
{
	errReason.clear();
	if (JVX_CHECK_SIZE_UNSELECTED(params.buffersize))
	{
		errReason = "Invalid buffersize specified.";
		goto fail;
	}
	if (JVX_CHECK_SIZE_UNSELECTED(params.rate))
	{
		errReason = "Invalid samplerate specified.";
		goto fail;
	}
	if (JVX_CHECK_SIZE_UNSELECTED(params.number_channels))
	{
		goto fail;
	}
	if (params.format == JVX_DATAFORMAT_NONE)
	{
		if (params.format_group != JVX_DATAFORMAT_GROUP_TRIGGER_ONLY)
		{
			errReason = "Invalid format specified.";
			goto fail;
		}
	}
	if (params.format_group == JVX_DATAFORMAT_GROUP_NONE)
	{
		errReason = "Invalid format group specified.";
		goto fail;
	}
	if (params.data_flow == JVX_DATAFLOW_DONT_CARE)
	{
		errReason = "Invalid format group specified.";
		goto fail;
	}
	return JVX_NO_ERROR;
fail:
	return JVX_ERROR_INVALID_ARGUMENT;
}

jvxCBitField jvx_check_differences(jvxLinkDataDescriptor_con_params& params_one, jvxLinkDataDescriptor_con_params& params_other, jvxCBitField checkThis)
{
	jvxCBitField out = 0;
	if (jvx_bitTest(checkThis, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_BUFFERSIZE_SHIFT))
	{
		if (params_one.buffersize != params_other.buffersize)
		{
			jvx_bitSet(out, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_BUFFERSIZE_SHIFT);
		}
	}
	if (jvx_bitTest(checkThis, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_DATAFLOW_SHIFT))
	{
		if (params_one.data_flow != params_other.data_flow)
		{
			jvx_bitSet(out, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_DATAFLOW_SHIFT);
		}
	}
	if (jvx_bitTest(checkThis, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_FORMAT_SHIFT))
	{
		if (params_one.format != params_other.format)
		{
			jvx_bitSet(out, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_FORMAT_SHIFT);
		}
	}
	if (jvx_bitTest(checkThis, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_NUM_CHANNELS_SHIFT))
	{
		if (params_one.number_channels != params_other.number_channels)
		{
			jvx_bitSet(out, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_NUM_CHANNELS_SHIFT);
		}
	}
	if (jvx_bitTest(checkThis, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_SAMPLERATE_SHIFT))
	{
		if (params_one.rate != params_other.rate)
		{
			jvx_bitSet(out, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_SAMPLERATE_SHIFT);
		}
	}
	if (jvx_bitTest(checkThis, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_SEGX_SHIFT))
	{
		if (params_one.segmentation.x != params_other.segmentation.x)
		{
			jvx_bitSet(out, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_SEGX_SHIFT);
		}
	}
	if (jvx_bitTest(checkThis, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_SEGY_SHIFT))
	{
		if (params_one.segmentation.y != params_other.segmentation.y)
		{
			jvx_bitSet(out, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_SEGY_SHIFT);
		}
	}
	if (jvx_bitTest(checkThis, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_SUBFORMAT_SHIFT))
	{
		if (params_one.format_group != params_other.format_group)
		{
			jvx_bitSet(out, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_SUBFORMAT_SHIFT);
		}
	}
	if (jvx_bitTest(checkThis, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_FORMATSPEC_SHIFT))
	{
		if (params_one.format_spec != params_other.format_spec)
		{
			jvx_bitSet(out, (jvxSize)jvxAddressLinkDataEntry::JVX_ADDRESS_FORMATSPEC_SHIFT);
		}
	}
	return out;
}

jvxSize jvx_derive_buffersize(jvxLinkDataDescriptor_con_params& params_one)
{
	jvxSize res = JVX_SIZE_UNSELECTED;
	if (params_one.format_group != JVX_DATAFORMAT_GROUP_NONE)
	{
		jvxSize szElement = jvxDataFormatGroup_getsize_mult(params_one.format_group);
		jvxSize szElementsLine = params_one.segmentation.x * szElement;
		jvxSize szElementsField = params_one.segmentation.y * szElementsLine;
		res = szElementsField / jvxDataFormatGroup_getsize_div(params_one.format_group);
	}
	return res;
}

jvxSize jvx_derive_buffersize_bytes(jvxLinkDataDescriptor_con_params& params_one)
{
	return jvx_derive_buffersize(params_one) * jvxDataFormat_getsize(params_one.format);
}

jvxData jvx_derive_elementsize(jvxDataFormatGroup format_group)
{
	jvxData res = -1;
	if (format_group != JVX_DATAFORMAT_GROUP_NONE) 
	{
		res = jvxDataFormatGroup_getsize_mult(format_group) / jvxDataFormatGroup_getsize_div(format_group);
	}
	return res;
}

jvxData jvx_derive_elementsize_byte(jvxDataFormatGroup format_group, jvxDataFormat format)
{
	return jvx_derive_elementsize(format_group) * jvxDataFormat_getsize(format);
}

	