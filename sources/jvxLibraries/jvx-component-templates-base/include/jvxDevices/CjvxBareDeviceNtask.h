#ifndef __CJVXBAREDEVICENTASK_H__
#define __CJVXBAREDEVICENTASK_H__

#include "jvx.h"
#include "common/CjvxObject.h"
#include "common/CjvxDevice.h"
#include "common/CjvxProperties.h"
#include "common/CjvxSequencerControl.h"
//#include "templates/common/CjvxInputOutputConnector.h"
#include "common/CjvxConnectorFactory.h"
#include "common/CjvxConnectionMaster.h"
#include "common/CjvxConnectorMasterFactory.h"
#include "common/jvxTypedefsNTask.h"
#include "common/jvxTypedefsMasterTask.h"

#include "jvxDevices/CjvxMaster_Ntask.h"

struct jvxOneMasterDefine
{
	jvxSize idMaster;
	std::string nmMaster;

	jvxOneMasterDefine()
	{
		idMaster = JVX_SIZE_UNSELECTED;
		// nmMaster;
	}
};

class CjvxBareDeviceNtask: public IjvxDevice, public CjvxDevice, public CjvxObject,
	public IjvxSequencerControl, public CjvxSequencerControl,
	public IjvxConnectorFactory, public CjvxConnectorFactory,
	public IjvxConnectionMasterFactory, public CjvxConnectionMasterFactory,
	public CjvxMasterNtask_report, public IjvxInputOutputConnectorNtask
{
protected:
	
	jvxOneMasterCollection _common_set_master_ntask;

	struct
	{
		std::vector<jvxOneConnectorTask> fixedTasks;
	}_common_set_connector_ntask;

	JVX_CALLINGCONVENTION CjvxBareDeviceNtask(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxBareDeviceNtask();
	
	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_ntask(
		jvxLinkDataDescriptor* theData_in, 
		jvxLinkDataDescriptor* theData_out,
		jvxSize idTask)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_ntask(
		jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, 
		jvxSize idTask)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;
	
	// All other functions in state machine
#define JVX_STATE_MACHINE_DEFINE_READY
#define JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS
#define JVX_STATE_MACHINE_DEFINE_STARTSTOP
#include "codeFragments/simplify/jvxStateMachine_simplify.h"
#undef JVX_STATE_MACHINE_DEFINE_STARTSTOP
#undef JVX_STATE_MACHINE_DEFINE_PREPAREPOSTPROCESS
#undef JVX_STATE_MACHINE_DEFINE_READY

	// All object related forwards
#include "codeFragments/simplify/jvxObjects_simplify.h"

	// Master factory
#include "codeFragments/simplify/jvxConnectorMasterFactory_simplify.h"

	// Interfaces and default implementations for connections
#include "codeFragments/simplify/jvxConnectorFactory_simplify.h"
	
	// Sequencer control
#include "codeFragments/simplify/jvxSequencerControl_simplify.h"

	// Interface redirections
#define JVX_INTERFACE_SUPPORT_CONNECTOR_MASTER_FACTORY
#define JVX_INTERFACE_SUPPORT_DATAPROCESSOR_CONNECT
#define JVX_INTERFACE_SUPPORT_DATAPROCESSOR
#define JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#define JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_DATAPROCESSOR_CONNECT
#undef JVX_INTERFACE_SUPPORT_DATAPROCESSOR
#undef JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#undef JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY
#undef JVX_INTERFACE_SUPPORT_CONNECTOR_MASTER_FACTORY

#include "codeFragments/simplify/jvxDevice_simplify.h"

	virtual jvxErrorType JVX_CALLINGCONVENTION test_chain_master_id(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxMasterNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_chain_master_id(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxMasterNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_chain_master_id(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxMasterNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_chain_master_id(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxMasterNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_chain_master_id(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxMasterNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION is_ready_master_id(jvxBool* is_ready, jvxApiString* reasonifnot, jvxSize idCtxt
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
};

#endif