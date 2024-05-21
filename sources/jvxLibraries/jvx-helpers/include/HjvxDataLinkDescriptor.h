#ifndef __HJVXDATALINKDESCRIPTOR_H__
#define __HJVXDATALINKDESCRIPTOR_H__

#ifdef JVX_COMPILE_SMALL
#include "jvx_small.h"
#else
#include "jvx.h"
#endif

jvxErrorType  jvx_initDataLinkDescriptor(jvxLinkDataDescriptor* theData);
jvxErrorType jvx_terminateDataLinkDescriptor(jvxLinkDataDescriptor* theData);

jvxErrorType jvx_allocateDataLinkDescriptorRouteChannels(jvxLinkDataDescriptor* theDataTo, 
	jvxLinkDataDescriptor* theDataFrom, jvxSize numChannelsCopy, jvxHandle**** bufToStore,
	jvxSize* ptrEntriesRoutes = nullptr, jvxSize numEntriesRoutes = 0, jvxBool routeOutputToInput = true);
jvxErrorType jvx_deallocateDataLinkDescriptorRouteChannels(jvxLinkDataDescriptor* theData, jvxHandle*** bufFromStorage);

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

void jvx_presetMasterOnPrepare(jvxLinkDataDescriptor& datOut);
void jvx_constrainIconOnPrepare(jvxLinkDataDescriptor* datIn, jvxBool clearFlagsBuffer, jvxSize num_additional_pipleline_stages = 0, jvxSize num_min_buffers_in = 1, jvxLinkDataDescriptor* datOut = nullptr);

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
	static jvxErrorType static_ ## fname(jvxHandle* priv, jvxLinkDataAttachedBuffer* elm); \
	jvxErrorType ic_ ## fname(jvxLinkDataAttachedBuffer* elm)

#define JVX_ATTACHED_LINK_DATA_RELEASE_CALLBACK_DEFINE(cname, fname) \
jvxErrorType \
cname::static_ ## fname(jvxHandle* priv, jvxLinkDataAttachedBuffer* elm) \
{ \
	cname* this_ptr = reinterpret_cast<cname*>(priv); \
	if (this_ptr) \
	{ \
		return this_ptr->ic_ ## fname(elm); \
	} \
	return JVX_ERROR_INVALID_ARGUMENT; \
} \
jvxErrorType \
cname::ic_## fname(jvxLinkDataAttachedBuffer* elm)

//! Allocate a list of "lost buffer" objects to be forwarded through the processing chain
void
jvxLinkDataAttachedLostFrames_allocateRuntime(
	std::list<jvxLinkDataAttached_oneFrameLostElement>& lst_cds,
	jvxHandle* prv_ptr,
	jvx_release_attached cb);

//! Deallocate a list of "lost buffer" objects to be forwarded through the processing chain
void
jvxLinkDataAttachedLostFrames_deallocateRuntime(
	std::list<jvxLinkDataAttached_oneFrameLostElement>& lst_cds);

//! Browse through the list of objects and return the first one that is available
jvxBool
jvxLinkDataAttachedLostFrames_updatePrepare(
	std::list<jvxLinkDataAttached_oneFrameLostElement>& lst_cds,
	jvxSize newCnt,
	jvxLinkDataAttachedLostFrames*& ptrOut);

//! Browse through the list and release the provided element
jvxBool
jvxLinkDataAttachedLostFrames_updateComplete(
	std::list<jvxLinkDataAttached_oneFrameLostElement>& lst_cds,
	jvxLinkDataAttachedLostFrames* elm);

// =================================================================================================

jvxErrorType jvx_shift_buffer_pipeline_idx_on_start(
	jvxLinkDataDescriptor* theData, jvxSize runtmeId,
	jvxSize pipeline_offset, jvxSize* idx_stage,
	jvxSize tobeAccessedByStage = 0,
	callback_process_start_in_lock clbk = NULL,
	jvxHandle* priv_ptr = NULL);

jvxErrorType jvx_shift_buffer_pipeline_idx_on_stop(jvxLinkDataDescriptor* theData,
	jvxSize idx_stage = JVX_SIZE_UNSELECTED, jvxBool shift_fwd = true, jvxSize tobeAccessedByStage = 0,
	callback_process_stop_in_lock clbk = NULL, jvxHandle* priv_ptr = NULL);

// =================================================================================================

jvxErrorType jvx_allocate_pipeline_and_buffers_prepare_to(jvxLinkDataDescriptor* theData
#ifdef JVX_GLOBAL_BUFFERING_VERBOSE
	, const char* pipe_ref
#endif
);

jvxErrorType jvx_allocate_pipeline_and_buffers_prepare_to_zerocopy(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out);

jvxErrorType jvx_deallocate_pipeline_and_buffers_postprocess_to(jvxLinkDataDescriptor* theData);

jvxErrorType jvx_deallocate_pipeline_and_buffers_postprocess_to_zerocopy(jvxLinkDataDescriptor* theData);

jvxLinkDataAttachedChain* jvx_attached_push_front(jvxLinkDataAttachedChain* old_first, jvxLinkDataAttachedChain* new_first);

jvxLinkDataAttachedChain* jvx_attached_pop_front(jvxLinkDataAttachedChain* old_first, jvxLinkDataAttachedChain** removed_first);

jvxErrorType jvx_check_valid(jvxLinkDataDescriptor_con_params& params, std::string& reason);

jvxCBitField jvx_check_differences(jvxLinkDataDescriptor_con_params& params_one, 
		jvxLinkDataDescriptor_con_params& params_other, jvxCBitField checkThis);

#endif
