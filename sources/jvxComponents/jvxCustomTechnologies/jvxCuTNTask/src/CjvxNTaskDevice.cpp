#include "CjvxNTaskDevice.h"

#define TIMEOUT_THREAD 3000

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE
{
#endif

#define strsize 32
static char str[strsize] = { 0 };
#define JVX_DARWIN_VERBOSE
#define JVX_TIMEOUT_EREIGNIS_GET_MS 200

#define JVX_CONNECTOR_SET_0 0
#define JVX_CONNECTOR_SET_1 1

// =====================================================================

CjvxNTaskDevice::CjvxNTaskDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxNBareTaskDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	jvxOneConnectorTask theConnectorTask;
	jvxOneConnectorDefine theConnector;
	jvxOneMasterTask theMasterTask;

	if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
	{
		jvx_lock_text_log(jvxrtst_bkp);
		jvxrtst << "::" << __FUNCTION__ << ": " << "constructor of " << _common_set.theDescriptor << " device component" << std::endl;
		jvx_unlock_text_log(jvxrtst_bkp);
	}

	// Prepare the connectors set 0
	theConnector.idCon = JVX_CONNECTOR_SET_0;
	theConnector.nmConnector = "connector_0";
	theConnectorTask.inputConnector = theConnector;
	theConnectorTask.outputConnector = theConnector;
	_common_set_connector_ntask.fixedTasks.push_back(theConnectorTask);

	theConnector.idCon = JVX_CONNECTOR_SET_1;
	theConnector.nmConnector = "connector_1";
	theConnectorTask.inputConnector = theConnector;
	theConnectorTask.outputConnector = theConnector;
	_common_set_connector_ntask.fixedTasks.push_back(theConnectorTask);

	theMasterTask.nmConnector = "connector_0";
	theMasterTask.idCntxt = JVX_CONNECTOR_SET_0;
	theMasterTask.nmMaster = "master_0";
	_common_set_master_ntask.fixedMaster.push_back(theMasterTask);

	theMasterTask.nmConnector = "connector_1";
	theMasterTask.idCntxt = JVX_CONNECTOR_SET_1;
	theMasterTask.nmMaster = "master_1";
	_common_set_master_ntask.fixedMaster.push_back(theMasterTask);
}

CjvxNTaskDevice::~CjvxNTaskDevice()
{
	_common_set_connector_ntask.fixedTasks.clear();
	_common_set_master_ntask.fixedMaster.clear();
}

jvxErrorType
CjvxNTaskDevice::activate()
{
	jvxErrorType res = CjvxBareDeviceNtask::activate();
	if (res == JVX_NO_ERROR)
	{
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvx_lock_text_log(jvxrtst_bkp);
			jvxrtst << "::" << __FUNCTION__ << ": " << "Activate " << _common_set.theDescriptor << " device component" << std::endl;
			jvx_unlock_text_log(jvxrtst_bkp);
		}

	}
	return res;
}

jvxErrorType
CjvxNTaskDevice::deactivate()
{
	jvxErrorType res = _pre_check_deactivate();
	jvxDspBaseErrorType resL;
	if (res == JVX_NO_ERROR)
	{
		if (jvxrtst_bkp.dbgModule && jvxrtst_bkp.dbgLevel > 3)
		{
			jvx_lock_text_log(jvxrtst_bkp);
			jvxrtst << "::" << __FUNCTION__ << ": " << "Deactivate " << _common_set.theDescriptor << " device component" << std::endl;
			jvx_unlock_text_log(jvxrtst_bkp);
		}

		res = CjvxBareDeviceNtask::deactivate();
		assert(res == JVX_NO_ERROR);
	}
	return res;
}

jvxErrorType
CjvxNTaskDevice::test_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
	jvxSize idTask, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	switch (idTask)
	{
	case JVX_CONNECTOR_SET_0:
		// Whatever needs to be done here!!!
		break;
	case JVX_CONNECTOR_SET_1:
		// Whatever needs to be done here!!!
		break;
	}

	// If we have only master connectors, this chain ends here!!!
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxNTaskDevice::prepare_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, 
	jvxLinkDataDescriptor* theData_out, jvxSize idTask, CjvxOutputConnectorNtask* refto 
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) 
{
	jvxErrorType res = JVX_NO_ERROR;
	switch (idTask)
	{
	case JVX_CONNECTOR_SET_0:
		// Whatever needs to be done here!!!
		break;
	case JVX_CONNECTOR_SET_1:
		// Whatever needs to be done here!!!
		break;
	}

	return res;
}

jvxErrorType
CjvxNTaskDevice::postprocess_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, 
	jvxSize idTask, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) 
{
	jvxErrorType res = JVX_NO_ERROR;
	switch (idTask)
	{
	case JVX_CONNECTOR_SET_0:
		// Whatever needs to be done here!!!
		break;
	case JVX_CONNECTOR_SET_1:
		// Whatever needs to be done here!!!
		break;
	}
	return res;
}

jvxErrorType
CjvxNTaskDevice::process_buffers_icon_ntask(jvxLinkDataDescriptor* theData_in, 
	jvxLinkDataDescriptor* theData_out, jvxSize idTask, CjvxOutputConnectorNtask* refto,
	jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	switch (idTask)
	{
	case JVX_CONNECTOR_SET_0:
		// Whatever needs to be done here!!!
		break;
	case JVX_CONNECTOR_SET_1:
		// Whatever needs to be done here!!!
		break;
	}
	return res;
}



// ======================================================================================================

jvxErrorType
CjvxNTaskDevice::test_chain_master_id(jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxMasterNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	switch (idCtxt)
	{
	case JVX_CONNECTOR_SET_0:
		// Whatever needs to be done here!!!
		break;
	case JVX_CONNECTOR_SET_1:
		// Whatever needs to be done here!!!
		break;
	}

	if (refto)
	{
		res = refto->test_chain_master_core(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

