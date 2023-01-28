#ifndef __CJVXNBARENTASK_H__
#define __CJVXNBARENTASK_H__

#include "CjvxNodeBaseNtask.h"
#include "common/jvxTypedefsNTask.h"

#include "CjvxInputOutputConnector_Ntask.h"
#include "common/CjvxNegotiate.h"

#define JVX_DEFAULT_CON_ID 0
#define JVX_DEFAULT_CON_VAR_OFF 256

#define JVX_NBARETASK_ATTACH_TASK(id, nm, master) \
{ \
	jvxOneConnectorTask theTask; \
	jvxOneConnectorDefine theConnector; \
	theConnector.assocMaster = static_cast<IjvxConnectionMaster*>(master); \
	/* Add the secondary plugs which are linked to the master functionality. */ \
	theConnector.idCon = id; \
	theConnector.nmConnector = nm; \
	theTask.inputConnector = theConnector; \
	theConnector.idCon = id; \
	theConnector.nmConnector = nm; \
	theTask.outputConnector = theConnector; \
	_common_set_node_ntask.fixedTasks.push_back(theTask); \
}

// Make this mode an audio node
class CjvxBareNtask : public CjvxNodeBaseNtask, public IjvxInputOutputConnectorNtask
{
protected:

	struct
	{
		std::vector<jvxOneConnectorTask> fixedTasks;
	} _common_set_node_ntask;

	struct buffer_cfg
	{
		jvxSize num_additional_pipleline_stages_cfg;
		jvxSize num_min_buffers_in_cfg;
		jvxSize num_min_buffers_out_cfg;
		jvxBool zeroCopyBuffering_cfg;
		buffer_cfg()
		{
			num_additional_pipleline_stages_cfg = 0;
			num_min_buffers_in_cfg = 1;
			num_min_buffers_out_cfg = 1;
			zeroCopyBuffering_cfg = false;
		};
	};

	struct
	{
		jvxLinkDataDescriptor* theData_out_master;
		jvxSize idx_stage_out_master;
		jvxLinkDataDescriptor* theData_in_master;
		jvxSize idx_stage_in_master;
		JVX_THREAD_ID thread_id_master;
	} _common_set_nvtask_proc;

	class master_settings
	{
	public:
		jvxSize bsize = JVX_SIZE_UNSELECTED;
		jvxSize rate = JVX_SIZE_UNSELECTED;
		jvxDataFormat format = JVX_DATAFORMAT_NONE;
		jvxDataFormatGroup format_group = JVX_DATAFORMAT_GROUP_NONE;
		void clear()
		{
			bsize = JVX_SIZE_UNSELECTED;
			rate = JVX_SIZE_UNSELECTED;
			format = JVX_DATAFORMAT_NONE;
			format_group = JVX_DATAFORMAT_GROUP_NONE;
		};
	};

	master_settings master;

protected:

	CjvxNegotiate_input neg_primary_input;
	CjvxNegotiate_output neg_primary_output;

	buffer_cfg cfg_default_buffer;
	jvxBool derived_zeroCopyBuffering_rt;
	jvxCBitField connections = 0;

public:
	JVX_CALLINGCONVENTION CjvxBareNtask(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxBareNtask();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION is_ready(jvxBool* isReady, jvxApiString* reason)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
		jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_ocon_ntask(jvxLinkDataDescriptor* theData_out,
		jvxSize idCtxt, IjvxInputConnector* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
		jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
		jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
		jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
		jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_icon_ntask(jvxLinkDataTransferType tp, jvxHandle* data,
		jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxOutputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon_ntask(jvxLinkDataTransferType tp, jvxHandle* data,
		jvxLinkDataDescriptor* theData_out, jvxLinkDataDescriptor* theData_in, jvxSize idCtxt, CjvxInputConnectorNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon_ntask(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out,
		jvxSize idCtxt, CjvxOutputConnectorNtask* refto, jvxSize mt_mask, jvxSize idx_stage)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_ocon_ntask(jvxSize idCtxt, jvxSize mt_mask, jvxSize idx_stage) override;

	jvxErrorType inform_chain_test(IjvxInputConnector* icon);

	virtual jvxErrorType prepare_connect_icon_ntask_core(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
		CjvxOutputConnectorNtask* refto, jvxBool& zeroCopyBuffering_rt, buffer_cfg& cfg_buffer, jvxSize id_task JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	virtual jvxErrorType start_connect_icon_ntask_core(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
		CjvxOutputConnectorNtask* refto, jvxSize id_task JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	virtual jvxErrorType stop_connect_icon_ntask_core(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
		CjvxOutputConnectorNtask* refto, jvxSize id_task JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	virtual jvxErrorType postprocess_connect_icon_ntask_core(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt,
		CjvxOutputConnectorNtask* refto, jvxBool& zeroCopyBuffering_rt, jvxSize id_task JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	std::string getConnectorName(jvxSize idCTxt);

	virtual jvxErrorType process_buffers_icon_nvtask_master(
		jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out,
		jvxSize mt_mask, jvxSize idx_stage);

	virtual jvxErrorType process_buffers_icon_ntask_attached(
		jvxSize ctxtId,
		jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out,
		jvxSize mt_mask, jvxSize idx_stage);

	virtual void activate_local_tasks();
	virtual void deactivate_local_tasks();

	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_ntask(
		jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out,
		jvxSize idTask)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_ntask(
		jvxLinkDataDescriptor* theData_in,
		jvxLinkDataDescriptor* theData_out, 
		jvxSize idTask)override;

#define JVX_INTERFACE_SUPPORT_DATAPROCESSOR_CONNECT
#define JVX_INTERFACE_SUPPORT_DATAPROCESSOR
#define JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_CONFIGURATION
#define JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_DATAPROCESSOR_CONNECT
#undef JVX_INTERFACE_SUPPORT_DATAPROCESSOR
#undef JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
#undef JVX_INTERFACE_SUPPORT_CONFIGURATION
#undef JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY
};

#endif
