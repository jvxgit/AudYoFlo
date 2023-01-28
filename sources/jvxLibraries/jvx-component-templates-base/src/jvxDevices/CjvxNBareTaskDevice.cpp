#include "jvxDevices/CjvxNBareTaskDevice.h"

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

CjvxNBareTaskDevice::CjvxNBareTaskDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxBareDeviceNtask(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
}

CjvxNBareTaskDevice::~CjvxNBareTaskDevice()
{
	_common_set_connector_ntask.fixedTasks.clear();
	_common_set_master_ntask.fixedMaster.clear();
}

// =============================================================================

jvxErrorType
CjvxNBareTaskDevice::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
				    jvxHandle* sectionToContainAllSubsectionsForMe, 
				    const char* filename, jvxInt32 lineno)
{
    std::vector<std::string> warns;
	jvxErrorType res = JVX_NO_ERROR;
    if (res == JVX_NO_ERROR)
	{
		if (_common_set_min.theState == JVX_STATE_ACTIVE)
		{

		}
	}
    return res;
}

jvxErrorType
CjvxNBareTaskDevice::get_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
					jvxHandle* sectionWhereToAddAllSubsections)
{
	jvxErrorType res = JVX_NO_ERROR;
    if (res == JVX_NO_ERROR)
    {
	
    }
    return res;
}

jvxErrorType
CjvxNBareTaskDevice::test_connect_ocon_ntask(jvxLinkDataDescriptor* theData_out, jvxSize idTask,
	IjvxInputConnector* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (refto)
	{
		res = refto->test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	return res;
}

jvxErrorType
CjvxNBareTaskDevice::start_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
	jvxSize idTask, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	return res;
}

jvxErrorType
CjvxNBareTaskDevice::stop_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
	jvxSize idTask, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) 
{
	jvxErrorType res = JVX_NO_ERROR;
	return res;
}

jvxErrorType
CjvxNBareTaskDevice::transfer_forward_icon_ntask(jvxLinkDataTransferType tp, jvxHandle* data,
	jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, 
	jvxSize idTask, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) 
{
	jvxErrorType res = JVX_NO_ERROR;
	return res;
}

jvxErrorType
CjvxNBareTaskDevice::transfer_backward_ocon_ntask(jvxLinkDataTransferType tp, jvxHandle* data,
	jvxLinkDataDescriptor* theData_out, jvxLinkDataDescriptor* theData_in, jvxSize idTask, 
	CjvxInputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) 
{
	jvxErrorType res = JVX_NO_ERROR;
	return res;
}


jvxErrorType
CjvxNBareTaskDevice::process_buffers_ocon_ntask(jvxSize idTask, jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	return res;
}

// ======================================================================================================

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

