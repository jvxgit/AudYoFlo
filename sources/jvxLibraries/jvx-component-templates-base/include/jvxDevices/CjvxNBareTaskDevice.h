#ifndef __CJVXNBARETASKDEVICE_H__
#define __CJVXNBARETASKDEVICE_H__

#include "jvxDevices/CjvxBareDeviceNtask.h"

#ifdef JVX_OS_LINUX
#include "unistd.h"
#include <fcntl.h>
#endif

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxNBareTaskDevice : public CjvxBareDeviceNtask,
	public IjvxProperties, public CjvxProperties,
	public IjvxConfiguration
{
private:
	

public:
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	// Component class interface member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	JVX_CALLINGCONVENTION CjvxNBareTaskDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxNBareTaskDevice() override;

	// ======================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION put_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionToContainAllSubsectionsForMe, 
		const char* filename, jvxInt32 lineno) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION get_configuration(jvxCallManagerConfiguration* callMan,
		IjvxConfigProcessor* processor,
		jvxHandle* sectionWhereToAddAllSubsections) override;

	// ======================================================

	// Interface properties
#include "codeFragments/simplify/jvxProperties_simplify.h"

		// interface referenes
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
#include "codeFragments/simplify/jvxSystemStatus_simplify.h"

	// =======================================================================================

	// This callback is within the test chain. If associated to a master, the refto pointer is zero, otherwise the chain must be moved forward
	/*
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon_id(jvxLinkDataDescriptor* theData_in, 
		jvxLinkDataDescriptor* theData_out, jvxSize idTask, CjvxOutputConnectorNtask* refto 
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is within the test chain. If associated to a master, the refto pointer is zero, otherwise the chain must be moved forward
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon_id(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idTask, CjvxOutputConnectorNtask* refto 
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is within the test chain. If associated to a master, the refto pointer is zero, otherwise the chain must be moved forward
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon_id(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idTask, CjvxOutputConnectorNtask* refto 
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;

	// This callback is within the test chain. If associated to a master, the refto pointer is zero, otherwise the chain must be moved forward
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon_id(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idTask, CjvxOutputConnectorNtask* refto) = 0;
	*/
  
	// ==================================================================================================

	// This callback is within the test chain. It is the task to move the chain forward in the callback - therefore the refto pointer is passed as an argument
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_ocon_ntask(jvxLinkDataDescriptor* theData_out,
		jvxSize idTask, IjvxInputConnector* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// This callback is within the test chain. It is the task to move the chain forward in the callback - therefore the refto pointer is passed as an argument
	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon_ntask(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idTask, CjvxOutputConnectorNtask* refto 
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// This callback is within the test chain. It is the task to move the chain forward in the callback - therefore the refto pointer is passed as an argument
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon_ntask(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idTask, CjvxOutputConnectorNtask* refto 
		JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_icon_ntask(jvxLinkDataTransferType tp,
		jvxHandle* data, jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, 
		jvxSize idTask, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon_ntask(jvxLinkDataTransferType tp,
		jvxHandle* data, jvxLinkDataDescriptor* theData_out, jvxLinkDataDescriptor* theData_in, 
		jvxSize idTask, CjvxInputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_ocon_ntask(jvxSize idTask,
		jvxSize mt_mask, jvxSize idx_stage) override;

#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_CONFIGURATION
#define JVX_INTERFACE_SUPPORT_BASE_CLASS CjvxBareDeviceNtask
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_BASE_CLASS
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
#undef JVX_INTERFACE_SUPPORT_CONFIGURATION

	// =======================================================================================
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif


