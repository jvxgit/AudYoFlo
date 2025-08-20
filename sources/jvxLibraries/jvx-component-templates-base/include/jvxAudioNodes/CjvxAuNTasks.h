#ifndef __CJVXAUNTASKS_H__
#define __CJVXAUNTASKS_H__

#include "jvx.h"
#include "jvxNodes/CjvxBareNtask.h"
#include "pcg_CjvxAuNTasks_pcg.h"
#include "common/CjvxNegotiate.h"

#define JVX_SHIFT_OFFSET_SUBMODULE_AUNTASKS 256

class CjvxAuNTasks : public CjvxBareNtask, public CjvxAuNTasks_pcg
{

protected:

	typedef enum 
	{
		JVX_AUNTASKS_SECONDARY_MASTER,
		JVX_AUNTASKS_PRIMARY_MASTER,
	} jvxAuNTasks_configFlowType;
		
	CjvxNegotiate_input neg_secondary_input;
	CjvxNegotiate_output neg_secondary_output;

	IjvxInputConnector* sec_input;
	IjvxInputConnector* pri_input;

	buffer_cfg cfg_sec_buffer;
	jvxBool sec_zeroCopyBuffering_rt;

	jvxAuNTasks_configFlowType parameter_config;

	struct
	{
		jvxBool buffersize_pri_to_sec;
		jvxBool rate_pri_to_sec;
	} auto_parameters;

	struct
	{
		jvxSize numGlitchesProcessing;
	} in_proc;

public:

	JVX_CALLINGCONVENTION CjvxAuNTasks(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);

	virtual JVX_CALLINGCONVENTION ~CjvxAuNTasks();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;

	virtual jvxErrorType test_connect_icon_ntask(jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
		CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType test_connect_ocon_ntask(jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
		IjvxInputConnector* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
		CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
		jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
		jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
		jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon_ntask(jvxLinkDataTransferType tp, jvxHandle* data, jvxLinkDataDescriptor* theData_out,
			jvxLinkDataDescriptor* theData_in, jvxSize idCtxt, CjvxInputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	jvxErrorType process_buffers_icon_nvtask_master(
		jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out,
		jvxSize mt_mask, jvxSize idx_stage)override;

	jvxErrorType process_buffers_icon_ntask_attached(
		jvxSize ctxtId,
		jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out,
		jvxSize mt_mask, jvxSize idx_stage)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION process_io(
		jvxLinkDataDescriptor* pri_in, jvxSize idx_stage_pri_in,
		jvxLinkDataDescriptor* sec_in, jvxSize idx_stage_sec_in,
		jvxLinkDataDescriptor* pri_out, jvxLinkDataDescriptor* sec_out);

private:
	
};

#endif
