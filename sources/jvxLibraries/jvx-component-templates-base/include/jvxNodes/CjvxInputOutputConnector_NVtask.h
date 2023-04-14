#ifndef __CJVXINPUTCONNECTORNVTASK_H__
#define __CJVXINPUTCONNECTORNVTASK_H__

#include "jvx.h"

class CjvxCommonNVtask
{
public:
	class common_set_comnvtask_tp
	{
	public:
		jvxBool detectLoop;
		IjvxObject* object;
		std::string descriptor;
		IjvxConnectorFactory* myParent;
		jvxSize myRuntimeId;
		IjvxDataConnectionCommon* theCommon;
		IjvxConnectionMaster* theMaster;
		IjvxConnectorBridge* conBridge;
		jvxSize ctxtTypeId;
		jvxSize ctxtId;
		jvxSize ctxtSubId;
		IjvxConnectionMaster* associatedMaster; // Set this to non-zero if this connector is associated to master functionality
		common_set_comnvtask_tp()
		{
			detectLoop = false;
			object = NULL;
			myParent = NULL;
			myRuntimeId = JVX_SIZE_UNSELECTED;
			theCommon = NULL;
			theMaster = NULL;
			conBridge = NULL;
			ctxtTypeId = JVX_SIZE_UNSELECTED;
			ctxtId = JVX_SIZE_UNSELECTED;
			ctxtSubId = JVX_SIZE_UNSELECTED;
			associatedMaster = NULL;
		};
	};
	
	common_set_comnvtask_tp _common_set_comnvtask;

public:
	CjvxCommonNVtask(jvxSize ctxId, const char* descr, IjvxConnectorFactory* par, 
		IjvxConnectionMaster* aM, IjvxObject* objRef, jvxSize ctxtSubId);

	void _modify_ctxtid(jvxSize cxtIdArg);
	jvxErrorType _descriptor_connector(jvxApiString* str);
	jvxErrorType _parent_factory(IjvxConnectorFactory** my_parent);
	jvxErrorType _associated_common(IjvxDataConnectionCommon** ref);

	jvxSize _context_subid()
	{
		return _common_set_comnvtask.ctxtSubId;
	};
};

// ==========================================================

class CjvxInputConnectorNVtask : public IjvxInputConnector
{
public:

	struct
	{
		jvxLinkDataDescriptor* theData_in;
		struct 
		{
			jvxDataFormatGroup format_group;
			jvxDataflow data_flow;
		} caps_in;
	} _common_set_icon_nvtask;

	CjvxCommonNVtask* _common_nvtask;

	CjvxInputConnectorNVtask();

	jvxErrorType JVX_CALLINGCONVENTION supports_connector_class_icon(
		jvxDataFormatGroup format_group,
		jvxDataflow data_flow)override;
	jvxErrorType JVX_CALLINGCONVENTION connected_ocon(IjvxOutputConnector** icon) override;

	// jvxErrorType _connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
};

// ==========================================================

class CjvxOutputConnectorNVtask : public IjvxOutputConnector
{
public:
	struct
	{
		jvxLinkDataDescriptor theData_out;
		struct {
			jvxDataFormatGroup format_group;
			jvxDataflow data_flow;
		}caps_out;
	} _common_set_ocon_nvtask;

	CjvxCommonNVtask* _common_nvtask;

	CjvxOutputConnectorNVtask();

	jvxErrorType JVX_CALLINGCONVENTION supports_connector_class_ocon(
		jvxDataFormatGroup format_group,
		jvxDataflow data_flow)override;

	// jvxErrorType outputs_data_format_group(jvxDataFormatGroup grp) override;
	jvxErrorType JVX_CALLINGCONVENTION connected_icon(IjvxInputConnector** icon) override;

	jvxErrorType _connect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	jvxErrorType _disconnect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	jvxErrorType _prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType _postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType _start_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType _stop_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	jvxErrorType _process_start_ocon(jvxSize pipeline_offset,
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk ,
		jvxHandle* priv_ptr);

	jvxErrorType _process_stop_ocon(
		jvxSize idx_stage,
		jvxBool shift_fwd,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock,
		jvxHandle* priv_ptr);

	jvxErrorType _process_buffers_ocon(
		jvxSize mt_mask, 
		jvxSize idx_stage);

	jvxErrorType _transfer_forward_ocon(
		jvxLinkDataTransferType tp, 
		jvxHandle* data, 
		JVX_CONNECTION_FEEDBACK_TYPE(fdb));

};
#endif
