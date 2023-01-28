#ifndef __HJVXDATALINKDESCRIPTOR_H__
#define __HJVXDATALINKDESCRIPTOR_H__

#ifdef JVX_COMPILE_SMALL
#include "jvx_small.h"
#else
#include "jvx.h"
#endif

jvxErrorType  jvx_initDataLinkDescriptor(jvxLinkDataDescriptor* theData);
jvxErrorType jvx_terminateDataLinkDescriptor(jvxLinkDataDescriptor* theData);

jvxErrorType jvx_allocateDataLinkDescriptor(jvxLinkDataDescriptor* theData, jvxBool allocateFields);
jvxErrorType jvx_deallocateDataLinkDescriptor(jvxLinkDataDescriptor* theData, jvxBool deallocateFields);

jvxErrorType jvx_copyDataLinkDescriptor(jvxLinkDataDescriptor* copyTo, jvxLinkDataDescriptor* copyFrom);
jvxErrorType jvx_ccopyDataLinkDescriptor(jvxLinkDataDescriptor* copyTo, jvxLinkDataDescriptor* copyFrom);

jvxErrorType jvx_copyDataLinkDescriptor_data(jvxLinkDataDescriptor* copyTo, jvxLinkDataDescriptor* copyFrom);

jvxErrorType jvx_allocateDataLinkDescriptor_oneExtBuffer(jvxLinkDataDescriptor_buf* theData, jvxSize idxBuf, jvxSize idxChan,
	jvxSize buffersize, jvxSize szElem, jvxByte*** raw, jvxByte*** net, jvxSize** offset, jvxSize* sz);
jvxErrorType jvx_deallocateDataLinkDescriptor_oneExtBuffer(jvxLinkDataDescriptor_buf* theData, jvxSize idxBuf, jvxSize idxChan, 
	jvxByte*** raw, jvxByte*** net, jvxSize** offset);

#ifndef JVX_COMPILE_SMALL
jvxErrorType jvx_allocateDataLinkPipelineControl(jvxLinkDataDescriptor* theData
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
	, const char* pipe_ref
#endif
);
jvxErrorType jvx_deallocateDataLinkPipelineControl(jvxLinkDataDescriptor* theData);
#endif

jvxErrorType jvx_allocateDataLinkSync(jvxLinkDataDescriptor* theData);
jvxErrorType jvx_deallocateDataLinkSync(jvxLinkDataDescriptor* theData);

jvxErrorType jvx_cinitDataLinkDescriptor(jvxLinkDataDescriptor* descr);

void jvx_neutralDataLinkDescriptor(jvxLinkDataDescriptor* theData, jvxBool sender);
void jvx_neutralDataLinkDescriptor_mem(jvxLinkDataDescriptor* theData, jvxBool sender);
void jvx_neutralDataLinkDescriptor_pipeline(jvxLinkDataDescriptor_con_pipeline* thePipeline);
void jvx_neutralDataLinkDescriptor_data(jvxLinkDataDescriptor_con_data* con_data);
void jvx_neutralDataLinkDescriptor_sync(jvxLinkDataDescriptor_con_sync* con_sync);

jvxErrorType jvx_copyDataLinkDescriptorSync(jvxLinkDataDescriptor* copyFrom, jvxSize idx_from,
	jvxLinkDataDescriptor* copyTo, jvxSize idx_to);

#define JVX_CHECK_PARAM_BUFFERSIZE_SHIFT 0
#define JVX_CHECK_PARAM_SAMPLERATE_SHIFT 1
#define JVX_CHECK_PARAM_FORMAT_SHIFT 2
#define JVX_CHECK_PARAM_NUM_CHANNELS_SHIFT 3
#define JVX_CHECK_PARAM_SEGMENTATION_X_SHIFT 4
#define JVX_CHECK_PARAM_SEGMENTATION_Y_SHIFT 5
#define JVX_CHECK_PARAM_SUBFORMAT_SHIFT 6

jvxBool jvx_check_in_out_params_match_test(jvxLinkDataDescriptor* cp_this, jvxLinkDataDescriptor* cp_that, jvxCBitField bfld);
jvxBool jvx_check_in_out_params_match_test_err(jvxLinkDataDescriptor* cp_this, jvxLinkDataDescriptor* cp_that, jvxCBitField bfld, std::string& errStr);

// =================================================================================================

#define JVX_PROCESS_BUFFERS_EXTRACT_IDX_INPUT(var, idx_stage, dataIn) \
	jvxSize var = idx_stage; \
	if (JVX_CHECK_SIZE_UNSELECTED(idx_stage_in)) \
	{ \
		idx_stage_in = *dataIn->con_pipeline.idx_stage_ptr; \
	}

// =================================================================================================
class jvxLinkDataAttached_oneFrameLostElement
{
public:
	jvxLinkDataAttachedLostFrames* ptr = nullptr;
	jvxBool inUse = false;
};

#define JVX_ATTACHED_LINK_DATA_MEMBER_LOST_FRAMES(var) std::list<jvxLinkDataAttached_oneFrameLostElement> var

#define JVX_ATTACHED_LINK_DATA_FORWARD_SINGLE(data_in, idxIn, data_out, idxOut) \
	if (data_in->con_data.attached_buffer_single[idxIn]) \
	{ \
		data_out->con_data.attached_buffer_single[idxOut] = \
				data_in->con_data.attached_buffer_single[idxIn]; \
		data_in->con_data.attached_buffer_single[idxIn] = NULL; \
	}

#define JVX_ATTACHED_LINK_DATA_RELEASE_CALLBACK_DECLARE(fname) \
	static jvxErrorType static_ ## fname(jvxHandle* priv, jvxLinkDataAttached* elm); \
	jvxErrorType ic_ ## fname(jvxLinkDataAttached* elm)

#define JVX_ATTACHED_LINK_DATA_RELEASE_CALLBACK_DEFINE(cname, fname) \
jvxErrorType \
cname::static_ ## fname(jvxHandle* priv, jvxLinkDataAttached* elm) \
{ \
	cname* this_ptr = reinterpret_cast<cname*>(priv); \
	if (this_ptr) \
	{ \
		return this_ptr->ic_ ## fname(elm); \
	} \
	return JVX_ERROR_INVALID_ARGUMENT; \
} \
jvxErrorType \
cname::ic_## fname(jvxLinkDataAttached* elm)

inline void
jvxLinkDataAttachedLostFrames_allocateRuntime(
	std::list<jvxLinkDataAttached_oneFrameLostElement>& lst_cds,
	jvxHandle* prv_ptr,
	jvx_release_attached cb)
{
	jvxLinkDataAttached_oneFrameLostElement theElm;
	theElm.ptr = NULL;
	JVX_DSP_SAFE_ALLOCATE_OBJECT(theElm.ptr, jvxLinkDataAttachedLostFrames);
	theElm.ptr->hdr.priv = prv_ptr;
	theElm.ptr->hdr.tp = JVX_LINKDATA_ATTACHED_REPORT_UPDATE_NUMBER_LOST_FRAMES;
	theElm.ptr->hdr.cb_release = cb;
	theElm.inUse = false;
	theElm.ptr->numLost = 0;
	lst_cds.push_back(theElm);
}

inline void
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

inline jvxBool
jvxLinkDataAttachedLostFrames_updatePrepare(
	std::list<jvxLinkDataAttached_oneFrameLostElement>& lst_cds,
	jvxSize newCnt, 
	jvxLinkDataAttached*& ptrOut)
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
			ptrOut = (jvxLinkDataAttached*)elm->ptr;
			elmentAvail = true;
			break;
		}
	}
	return elmentAvail;
}

inline jvxBool
jvxLinkDataAttachedLostFrames_updateComplete(
	std::list<jvxLinkDataAttached_oneFrameLostElement>& lst_cds,
	jvxLinkDataAttached* elm)
{
	jvxBool elmentExists = false;
	auto elml = lst_cds.begin();
	for (; elml != lst_cds.end(); elml++)
	{
		if (elm == (jvxLinkDataAttached*)elml->ptr)
		{
			elml->inUse = false;
			elmentExists = true;
			break;
		}
	}
	return elmentExists;
}

// =================================================================================================

inline jvxErrorType jvx_shift_buffer_pipeline_idx_on_start(
	jvxLinkDataDescriptor* theData, jvxSize runtmeId,
	jvxSize pipeline_offset, jvxSize* idx_stage, 
	jvxSize tobeAccessedByStage = 0,
	callback_process_start_in_lock clbk = NULL,
	jvxHandle* priv_ptr = NULL)
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

inline jvxErrorType jvx_shift_buffer_pipeline_idx_on_stop(jvxLinkDataDescriptor* theData, 
	jvxSize idx_stage = JVX_SIZE_UNSELECTED, jvxBool shift_fwd = true, jvxSize tobeAccessedByStage = 0, 
	callback_process_stop_in_lock clbk = NULL, jvxHandle* priv_ptr = NULL)
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

inline jvxErrorType  jvx_allocate_pipeline_and_buffers_prepare_to(jvxLinkDataDescriptor* theData
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

inline jvxErrorType JVX_CALLINGCONVENTION jvx_allocate_pipeline_and_buffers_prepare_to_zerocopy(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out)
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

inline jvxErrorType jvx_deallocate_pipeline_and_buffers_postprocess_to(jvxLinkDataDescriptor* theData)
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

inline jvxErrorType JVX_CALLINGCONVENTION jvx_deallocate_pipeline_and_buffers_postprocess_to_zerocopy(jvxLinkDataDescriptor* theData)
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

inline jvxLinkDataAttached*
jvx_attached_push_front(jvxLinkDataAttached* old_first, jvxLinkDataAttached* new_first)
{
	jvxLinkDataAttached* retVal = new_first;
	new_first->next = old_first;
	return retVal;
}

inline jvxLinkDataAttached*
jvx_attached_pop_front(jvxLinkDataAttached* old_first, jvxLinkDataAttached** removed_first)
{
	jvxLinkDataAttached* retVal = old_first->next;
	old_first->next = NULL;
	if (removed_first)
	{
		*removed_first = old_first;
	}
	return retVal;
}
#endif
