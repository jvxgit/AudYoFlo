#include "jvxDevices/CjvxMaster_Ntask.h"

CjvxMasterNtask::CjvxMasterNtask(jvxSize ctxId, const char* descr,
	IjvxConnectionMasterFactory* par,
	CjvxMasterNtask_report* report, CjvxObject* objRef) : CjvxConnectionMaster()
{
	_init_master(objRef, descr, par);
	_common_set_ld_master_ntask.cbRef = report;
	_common_set_ld_master_ntask.ctxtId = ctxId;
	_common_set_ld_master_ntask.connOut = NULL;
	_common_set_ld_master_ntask.connIn = NULL;

}

CjvxMasterNtask::~CjvxMasterNtask()
{
	_common_set_ld_master_ntask.cbRef = NULL;
	_common_set_ld_master_ntask.ctxtId = JVX_SIZE_UNSELECTED;
}
	

jvxErrorType
CjvxMasterNtask::link_master(CjvxOutputConnectorNtask* conn_out, CjvxInputConnectorNtask* conn_in)
{
	if (
		(_common_set_ld_master_ntask.connOut == NULL) &&
		(_common_set_ld_master_ntask.connIn == NULL))
	 {
		_common_set_ld_master_ntask.connOut = conn_out;
		_common_set_ld_master_ntask.connIn = conn_in;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxMasterNtask::unlink_master()
{
	if (
		(_common_set_ld_master_ntask.connOut) && 
		(_common_set_ld_master_ntask.connIn))
	{
		_common_set_ld_master_ntask.connOut = NULL;
		_common_set_ld_master_ntask.connIn = NULL;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CjvxMasterNtask::test_chain_master_core(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return _test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

// ==============================================================
jvxErrorType
CjvxMasterNtask::test_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_ld_master_ntask.cbRef)
	{
		res = _common_set_ld_master_ntask.cbRef->test_chain_master_id(
			_common_set_ld_master_ntask.connIn->_common_set_icon_nvtask.theData_in,
			&_common_set_ld_master_ntask.connOut->_common_set_ocon_nvtask.theData_out,
			_common_set_ld_master_ntask.ctxtId, this JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	else
	{
		res = _test_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

jvxErrorType
CjvxMasterNtask::prepare_chain_master_core(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return _prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType
CjvxMasterNtask::prepare_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_ld_master_ntask.cbRef)
	{
		res = _common_set_ld_master_ntask.cbRef->prepare_chain_master_id(
			_common_set_ld_master_ntask.connIn->_common_set_icon_nvtask.theData_in, 
			&_common_set_ld_master_ntask.connOut->_common_set_ocon_nvtask.theData_out,
			_common_set_ld_master_ntask.ctxtId, this JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	else
	{
		res = _prepare_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

// ================================================================================
jvxErrorType
CjvxMasterNtask::postprocess_chain_master_core(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType
CjvxMasterNtask::postprocess_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_ld_master_ntask.cbRef)
	{
		res = _common_set_ld_master_ntask.cbRef->postprocess_chain_master_id(
			_common_set_ld_master_ntask.connIn->_common_set_icon_nvtask.theData_in, 
			&_common_set_ld_master_ntask.connOut->_common_set_ocon_nvtask.theData_out,
			_common_set_ld_master_ntask.ctxtId, this JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	else
	{ 
		res = _postprocess_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(res == JVX_NO_ERROR);
	}
	return res;
}

// ================================================================================

jvxErrorType
CjvxMasterNtask::start_chain_master_core(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return _start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType
CjvxMasterNtask::start_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_ld_master_ntask.cbRef)
	{
		res = _common_set_ld_master_ntask.cbRef->start_chain_master_id(
			_common_set_ld_master_ntask.connIn->_common_set_icon_nvtask.theData_in, 
			&_common_set_ld_master_ntask.connOut->_common_set_ocon_nvtask.theData_out,
			_common_set_ld_master_ntask.ctxtId, this JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	else
	{
		res = _start_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

// ================================================================================#

jvxErrorType
CjvxMasterNtask::stop_chain_master_core(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return _stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType
CjvxMasterNtask::stop_chain_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_ld_master_ntask.cbRef)
	{
		res = _common_set_ld_master_ntask.cbRef->stop_chain_master_id(
			_common_set_ld_master_ntask.connIn->_common_set_icon_nvtask.theData_in, 
			&_common_set_ld_master_ntask.connOut->_common_set_ocon_nvtask.theData_out,
			_common_set_ld_master_ntask.ctxtId, this JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	}
	else
	{
		res = _stop_chain_master(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(res == JVX_NO_ERROR);
	}
	return res;
}

jvxErrorType 
CjvxMasterNtask::transfer_chain_forward_master(jvxLinkDataTransferType tp,
	jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	if (tp == JVX_LINKDATA_TRANSFER_ASK_COMPONENTS_READY)
	{
		if (_common_set_ld_master_ntask.cbRef)
		{
			jvxErrorType res = JVX_NO_ERROR;
			jvxBool isReady = false;
			jvxApiString* astr = (jvxApiString*)data;

			_common_set_ld_master_ntask.cbRef->is_ready_master_id(&isReady, astr,
				_common_set_ld_master_ntask.ctxtId	JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
			assert(res == JVX_NO_ERROR);

			if (isReady == false)
			{
				return JVX_ERROR_NOT_READY;
			}
		}
	}
	return _transfer_chain_forward_master(tp, data JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
};