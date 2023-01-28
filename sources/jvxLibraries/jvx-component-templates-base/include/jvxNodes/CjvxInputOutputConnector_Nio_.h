#ifndef __CJVXINPUTOUTPUT_NIO_H__
#define __CJVXINPUTOUTPUT_NIO_H__

#include "jvx.h"

#if 0 // <- currently not ready

class CjvxInputConnectorNio;
class CjvxOutputConnectorNio;

JVX_INTERFACE CjvxCommonNio_report
{
public:
	virtual JVX_CALLINGCONVENTION ~CjvxCommonNio_report() {};

	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon_id(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt_in, jvxSize idCtxt_out, CjvxOutputConnectorNio* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon_id(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt_in, jvxSize idCtxt_out, CjvxOutputConnectorNio* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon_id(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt_in, jvxSize idCtxt_out, CjvxOutputConnectorNio* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon_id(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt_in, jvxSize idCtxt_out, CjvxOutputConnectorNio* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon_id(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt_in, jvxSize idCtxt_out, CjvxOutputConnectorNio* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_icon_id(jvxLinkDataTransferType tp, jvxHandle* data, jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt_in, jvxSize idCtxt_out, CjvxOutputConnectorNio* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon_id(jvxLinkDataTransferType tp, jvxHandle* data, jvxLinkDataDescriptor* theData_out, jvxLinkDataDescriptor* theData_in, jvxSize idCtxt_in, jvxSize idCtxt_out, CjvxInputConnectorNio* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon_id(jvxLinkDataDescriptor* theData_in, jvxLinkDataDescriptor* theData_out, jvxSize idCtxt_in, jvxSize idCtxt_out, CjvxOutputConnectorNio* refto) = 0;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_ocon_id(jvxSize idCtxt, jvxSize * mt_mask) = 0;
};

class CjvxCommonNio
{
public:
	struct
	{
		jvxBool detectLoop;
		IjvxObject* object;
		std::string descriptor;
		IjvxConnectorFactory* myParent;
		jvxSize myRuntimeId;
		IjvxDataConnectionCommon* theCommon;
		IjvxConnectionMaster* theMaster;
		IjvxConnectorBridge* conBridge;
		jvxSize ctxtId;
		CjvxCommonNio_report* cbRef;
	} _common_set_comnio;
	IjvxConnectionMaster* associatedMaster;

public:
	CjvxCommonNio(jvxSize ctxId, const char* descr, IjvxConnectorFactory* par, CjvxCommonNio_report* report, IjvxConnectionMaster* aM);
	~CjvxCommonNio();
	virtual jvxErrorType JVX_CALLINGCONVENTION _descriptor_connector(jvxApiString* str);
	virtual jvxErrorType JVX_CALLINGCONVENTION _parent_factory(IjvxConnectorFactory** my_parent);
};

class CjvxInputConnectorNio : public IjvxInputConnector, public CjvxCommonNio
{
	friend class CjvxOutputConnectorNio;

protected:
	struct
	{
		std::vector<CjvxOutputConnectorNio*> ocons;
		jvxLinkDataDescriptor* theData_in;
		jvxSize num_min_buffers_in;
		jvxSize num_additional_pipleline_stages;
	} _common_set_icon_nio;

public:

	CjvxInputConnectorNio(jvxSize ctxId, const char* descr, IjvxConnectorFactory* par, CjvxCommonNio_report* report, IjvxConnectionMaster* aM);
	~CjvxInputConnectorNio();

	// =======================================================================================================
	jvxErrorType add_reference(CjvxOutputConnectorNio*ref);
	// =======================================================================================================
	
	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_connector(jvxApiString* str) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION parent_factory(IjvxConnectorFactory** my_parent) override;
	// =======================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION associated_common_icon(IjvxDataConnectionCommon** ref)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION select_connect_icon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect_connect_icon(IjvxConnectorBridge* bri)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(var))override;

	// ==========================================================================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(jvxSize pipeline_offset = 0, jvxSize* idx_stage = NULL)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask = JVX_SIZE_UNSELECTED)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(jvxSize idx_stage = JVX_SIZE_UNSELECTED, jvxBool shift_fwd = true)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_icon(jvxLinkDataTransferType tp, jvxHandle*, JVX_CONNECTION_FEEDBACK_TYPE(var))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle*, JVX_CONNECTION_FEEDBACK_TYPE(var))override;
};

class CjvxOutputConnectorNio : public IjvxOutputConnector, public CjvxCommonNio
{
	friend class CjvxInputConnectorNio;
protected:
	struct
	{
		std::vector<CjvxInputConnectorNio*> icons;
		jvxLinkDataDescriptor theData_out;
		CjvxInputConnectorNio* icon_connect;
	} _common_set_ocon_nio;
public:

	CjvxOutputConnectorNio(jvxSize ctxId, const char* descr, IjvxConnectorFactory* par, CjvxCommonNio_report* report, IjvxConnectionMaster* aM);

	// =======================================================================================================
	jvxErrorType add_reference(CjvxInputConnectorNio*ref);

	// =======================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_connector(jvxApiString* str)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION parent_factory(IjvxConnectorFactory** my_parent)override;
	// =======================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION associated_common_ocon(IjvxDataConnectionCommon** ref)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION select_connect_ocon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect_connect_ocon()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_ocon(IjvxConnectionMaster* theMaster JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(var))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_ocon(jvxSize pipeline_offset = 0, jvxSize* idx_stage = NULL)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_ocon()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_ocon(jvxSize idx_stage = JVX_SIZE_UNSELECTED, jvxBool shift_fwd = true)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle*, JVX_CONNECTION_FEEDBACK_TYPE(var))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_ocon(jvxLinkDataTransferType tp, jvxHandle*, JVX_CONNECTION_FEEDBACK_TYPE(var))override;
};

#endif
#endif