#include "CjvxSignalProcessingDeviceDeploy.h"

CjvxSignalProcessingDeviceDeploy::CjvxSignalProcessingDeviceDeploy(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxFullMasterDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	interactRef = NULL;
	JVX_INITIALIZE_MUTEX(safeAccessChain);
};

CjvxSignalProcessingDeviceDeploy::~CjvxSignalProcessingDeviceDeploy()
{
	JVX_TERMINATE_MUTEX(safeAccessChain);
}

jvxErrorType 
CjvxSignalProcessingDeviceDeploy::activate()
{
	jvxErrorType res = CjvxFullMasterDevice::activate();
	if (res == JVX_NO_ERROR)
	{
		CjvxSignalProcessingDeviceDeploy_genpcg::init_all();
		CjvxSignalProcessingDeviceDeploy_genpcg::allocate_all();
		CjvxSignalProcessingDeviceDeploy_genpcg::register_all(static_cast<CjvxProperties*>(this));
		CjvxSignalProcessingDeviceDeploy_genpcg::register_callbacks(static_cast<CjvxProperties*>(this),
			get_interface_ptr, reinterpret_cast<jvxHandle*>(this),
			NULL);
	}
	return res;
}

jvxErrorType 
CjvxSignalProcessingDeviceDeploy::deactivate()
{
	jvxErrorType res = CjvxFullMasterDevice::_pre_check_deactivate();
	if (res == JVX_NO_ERROR)
	{
		CjvxSignalProcessingDeviceDeploy_genpcg::unregister_callbacks(static_cast<CjvxProperties*>(this), NULL);
		CjvxSignalProcessingDeviceDeploy_genpcg::unregister_all(static_cast<CjvxProperties*>(this));
		CjvxSignalProcessingDeviceDeploy_genpcg::deallocate_all();

		res = CjvxFullMasterDevice::deactivate();
	}
	return res;
}

// ===============================================================================================

jvxErrorType
CjvxSignalProcessingDeviceDeploy::prepare()
{
	jvxErrorType res = CjvxFullMasterDevice::prepare();
	if (res == JVX_NO_ERROR)
	{
		jvxLinkDataAttachedLostFrames_allocateRuntime(
			lst_lost,
			reinterpret_cast<jvxHandle*>(this),
			static_release_lost_frames);
		lost_cnt_new = 0;
		lost_cnt_reported = 0;
	}
	return res;
}

jvxErrorType
CjvxSignalProcessingDeviceDeploy::postprocess()
{
	jvxErrorType res = CjvxFullMasterDevice::postprocess();
	if (res == JVX_NO_ERROR)
	{
		jvxLinkDataAttachedLostFrames_deallocateRuntime(
			lst_lost);
	}
	return res;
}

// ==================================================================================

jvxErrorType
CjvxSignalProcessingDeviceDeploy::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	res = CjvxFullMasterDevice::test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	return res;
}

jvxErrorType
CjvxSignalProcessingDeviceDeploy::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(safeAccessChain);
	res = CjvxFullMasterDevice::start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	JVX_UNLOCK_MUTEX(safeAccessChain);
	return res;
}

jvxErrorType
CjvxSignalProcessingDeviceDeploy::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_LOCK_MUTEX(safeAccessChain);
	res = CjvxFullMasterDevice::stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	JVX_UNLOCK_MUTEX(safeAccessChain);
	return res;
}

jvxErrorType
CjvxSignalProcessingDeviceDeploy::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = CjvxFullMasterDevice::process_buffers_icon(mt_mask, idx_stage);
	if (res == JVX_NO_ERROR)
	{
		JVX_PROCESS_BUFFERS_EXTRACT_IDX_INPUT(idx_stage_in, idx_stage, _common_set_ldslave.theData_in);

		if (_common_set_ldslave.theData_in->con_data.attached_buffer_single[idx_stage_in])
		{
			jvxLinkDataAttached* ptr = _common_set_ldslave.theData_in->con_data.attached_buffer_single[idx_stage_in];
			ptr->cb_release(ptr->priv,
				ptr);
			_common_set_ldslave.theData_in->con_data.attached_buffer_single[idx_stage_in] = NULL;
		}
	}
	return res;
}

JVX_PROPERTIES_FORWARD_C_CALLBACK_EXECUTE_FULL(CjvxSignalProcessingDeviceDeploy, get_interface_ptr)
{
	const jPRG* ptr = castPropRawPointer<const jPRG>(rawPtr, JVX_DATAFORMAT_NONE);
	assert(ptr);
	IjvxSignalProcessingDeploy** fldIf = (IjvxSignalProcessingDeploy**)ptr->raw();
	if (fldIf)
		*fldIf = static_cast<IjvxSignalProcessingDeploy*>(this);

	return JVX_NO_ERROR;
}

JVX_ATTACHED_LINK_DATA_RELEASE_CALLBACK_DEFINE(CjvxSignalProcessingDeviceDeploy, release_lost_frames)
{
	jvxLinkDataAttachedLostFrames_updateComplete(
		lst_lost,
		elm);
	return JVX_NO_ERROR;
}