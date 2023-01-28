#ifndef __CJVXNTASKDEVICE_H__
#define __CJVXNTASKDEVICE_H__

#include "jvxDevices/CjvxNBareTaskDevice.h"

#ifdef JVX_OS_LINUX
#include "unistd.h"
#include <fcntl.h>
#endif

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxNTaskDevice: public CjvxNBareTaskDevice
{
private:
	

public:
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	// Component class interface member functions
	// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	JVX_CALLINGCONVENTION CjvxNTaskDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxNTaskDevice() override;

	virtual jvxErrorType JVX_CALLINGCONVENTION activate() override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate() override;

	// =======================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION test_chain_master_id(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxMasterNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon_ntask(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idTask, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon_ntask(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idTask, CjvxOutputConnectorNtask* refto	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon_ntask(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idTask, CjvxOutputConnectorNtask* refto	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon_ntask(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idTask, CjvxOutputConnectorNtask* refto,
		jvxSize mt_mask, jvxSize idx_stage) override;

	
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif


