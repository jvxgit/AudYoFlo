#ifndef __CJVXGENERICWRAPPERDEVICE_HOSTRELOCATOR__H__
#define __CJVXGENERICWRAPPERDEVICE_HOSTRELOCATOR__H__

#include "jvx.h"

#include "common/CjvxInputOutputConnector.h"
#include "common/CjvxConnectorFactory.h"
#include "jvxHosts/CjvxDataConnections.h"

class CjvxGenericWrapperDevice;

class CjvxGenericWrapperDevice_hostRelocator: 
	public IjvxHiddenInterface, public IjvxReport,
	public IjvxConnectorFactory, public CjvxConnectorFactory,
	public IjvxInputConnector, public IjvxOutputConnector, public CjvxInputOutputConnector,
	public IjvxDataConnections, public CjvxDataConnections
{
	friend class CjvxGenericWrapperDevice;

private:
	struct
	{
		CjvxGenericWrapperDevice* refDevice;
		IjvxReport* refReport;
        int cntRefReport;
	} runtime;

	IjvxConnectorFactory* theConnFac_dev;
	IjvxConnectionMasterFactory* theMasterFac_dev;
	IjvxConnectorFactory* theConnFac_me;
	IjvxInputConnectorSelect* icon_dev;
	IjvxInputConnectorSelect* icon_me;
	IjvxConnectionMaster* mas_dev;
	IjvxOutputConnectorSelect* ocon_dev;
	IjvxOutputConnectorSelect* ocon_me;
	jvxSize id_connection_process;
	IjvxDataConnectionProcess* hdl_connection_process;
	jvxSize id_device2me;
	jvxSize id_me2device;
	std::string moduleNameLocal;
public:
	CjvxGenericWrapperDevice_hostRelocator();
	void setReference(CjvxGenericWrapperDevice* refDevice);
	void resetReference();
	~CjvxGenericWrapperDevice_hostRelocator();

	jvxErrorType transfer_backward_x(jvxLinkDataDescriptor* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));

	virtual jvxErrorType JVX_CALLINGCONVENTION supports_connector_class_ocon(
		jvxDataFormatGroup format_group,
		jvxDataflow data_flow)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION supports_connector_class_icon(
		jvxDataFormatGroup format_group,
		jvxDataflow data_flow)override;

	jvxErrorType activate();
	jvxErrorType deactivate();

	jvxErrorType x_select_master();
	jvxErrorType x_unselect_master();

	jvxErrorType x_connect_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType x_disconnect_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	jvxErrorType x_prepare_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType x_postprocess_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	jvxErrorType x_start_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType x_stop_chain(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	jvxErrorType x_transfer_forward_master(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	jvxErrorType x_test_master(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

#define JVX_CONNECT_X_FORWARD_MASTER
#include "codeFragments/simplify/jvxInputOutputConnectorX_simplify.h"
#undef JVX_CONNECT_X_FORWARD_MASTER

	jvxErrorType x_test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));
	jvxErrorType x_test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	// ==============================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION connect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
	{
		return _connect_connect_ocon(args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION disconnect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override
	{
		return _disconnect_connect_ocon(args JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	};

	jvxErrorType prepare_connect_icon_x(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	jvxErrorType postprocess_connect_icon_x(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb));
	jvxErrorType test_connect_icon_x(JVX_CONNECTION_FEEDBACK_TYPE(fdb));

	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(var)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(var)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(var)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION transfer_forward_ocon(jvxLinkDataTransferType tp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(var)) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	// ==============================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_icon(jvxSize pipeline_offset , 
		jvxSize* idx_stage, jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk,
		jvxHandle* priv_ptr)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;
	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_icon(jvxSize idx_stage , 
		jvxBool shift_fwd,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock clbk,
		jvxHandle* priv_ptr)override;

	// ==============================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override
	{
		return _prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
	{
		return _postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION start_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
	{
		return _start_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	};

	// =======================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION process_start_ocon(jvxSize pipeline_offset , 
		jvxSize* idx_stage,
		jvxSize tobeAccessedByStage,
		callback_process_start_in_lock clbk,
		jvxHandle* priv_ptr)override
	{
		return _process_start_ocon(pipeline_offset, idx_stage,
			tobeAccessedByStage,
			clbk, priv_ptr);
	};


	virtual jvxErrorType JVX_CALLINGCONVENTION process_buffers_ocon(jvxSize mt_mask, jvxSize idx_stage)override
	{
		return _process_buffers_ocon( mt_mask,  idx_stage);
	};


	virtual jvxErrorType JVX_CALLINGCONVENTION process_stop_ocon(jvxSize idx_stage , jvxBool shift_fwd,
		jvxSize tobeAccessedByStage,
		callback_process_stop_in_lock clbk,
		jvxHandle* priv_ptr)override
	{
		return _process_stop_ocon(idx_stage, shift_fwd,tobeAccessedByStage, clbk, priv_ptr);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION stop_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override
	{
		return _stop_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION select_connect_icon(IjvxConnectorBridge* obj, 
		IjvxConnectionMaster* master, IjvxDataConnectionCommon* ass_connection_common,
		IjvxInputConnector** replace_connector) override
	{
		return _select_connect_icon(obj, master, ass_connection_common, replace_connector);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION select_connect_ocon(
		IjvxConnectorBridge* obj, IjvxConnectionMaster* master, IjvxDataConnectionCommon* ass_connection_common,
		IjvxOutputConnector** replace_connector) override
	{
		return _select_connect_ocon(obj, master, ass_connection_common, replace_connector);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION unselect_connect_icon(IjvxConnectorBridge* obj,
		IjvxInputConnector* replace_connector) override
	{
		return _unselect_connect_icon(obj, replace_connector);
	};
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect_connect_ocon(IjvxConnectorBridge* obj,
		IjvxOutputConnector* replace_connector) override
	{
		return _unselect_connect_ocon(obj, replace_connector);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION associated_common_ocon(IjvxDataConnectionCommon **ref) override
	{
		return _associated_common_ocon(ref);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION associated_common_icon(IjvxDataConnectionCommon **ref) override
	{
		return _associated_common_icon(ref);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_connector(jvxApiString* str) override
	{
		return _descriptor_connector(str);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION parent_factory(IjvxConnectorFactory** my_parent)override
	{
		return _parent_factory(my_parent);
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION reference_component(
		jvxComponentIdentification* cpTp,
		jvxApiString* modName,
		jvxApiString* lContext) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION connected_icon(IjvxInputConnector** icon) override
	{
		return _connected_icon(icon);
	}

	virtual jvxErrorType JVX_CALLINGCONVENTION connected_ocon(IjvxOutputConnector** ocon) override
	{
		return _connected_ocon(ocon);
	}

	virtual IjvxInputConnector* reference_icon() override
	{
		return this;
	}

	virtual IjvxOutputConnector* reference_ocon() override
	{
		return this;
	}

	jvxErrorType request_trigger_itcon(IjvxTriggerInputConnector** itcon) override
	{
		return JVX_ERROR_UNSUPPORTED;
	}

	jvxErrorType return_trigger_itcon(IjvxTriggerInputConnector* itcon) override
	{
		return JVX_ERROR_UNSUPPORTED;
	}

	jvxErrorType request_trigger_otcon(IjvxTriggerOutputConnector** otcon)
	{
		return JVX_ERROR_UNSUPPORTED;
	}

	jvxErrorType return_trigger_otcon(IjvxTriggerOutputConnector* otcon)
	{
		return JVX_ERROR_UNSUPPORTED;
	}


	// =========================================================
	virtual jvxErrorType JVX_CALLINGCONVENTION request_hidden_interface(jvxInterfaceType, jvxHandle**)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION return_hidden_interface(jvxInterfaceType, jvxHandle*)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION object_hidden_interface(IjvxObject** objRef) override{ return JVX_ERROR_UNSUPPORTED; };

	virtual jvxErrorType JVX_CALLINGCONVENTION report_simple_text_message(const char* txt, jvxReportPriority prio)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_internals_have_changed(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxPropertyCategoryType cat, jvxSize propId, bool onlyContent, jvxSize offs, jvxSize num, const jvxComponentIdentification& tpTo,
		jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_take_over_property(const jvxComponentIdentification& thisismytype, IjvxObject* thisisme,
		jvxHandle* fld, jvxSize numElements, jvxDataFormat format, jvxSize offsetStart, jvxBool onlyContent,
		jvxPropertyCategoryType cat, jvxSize propId, const jvxComponentIdentification& tpTo,
		jvxPropertyCallPurpose callpurpose = JVX_PROPERTY_CALL_PURPOSE_NONE_SPECIFIC)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION report_command_request(jvxCBitField request,
		jvxHandle* caseSpecificData,
		jvxSize szSpecificData)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION request_command(const CjvxReportCommandRequest& request) override;

    virtual jvxErrorType JVX_CALLINGCONVENTION report_os_specific(jvxSize commandId, void* context)override;

	virtual jvxErrorType JVX_CALLINGCONVENTION interface_sub_report(IjvxSubReport** subReport) override;

	// ====================================================================================================

	virtual jvxErrorType JVX_CALLINGCONVENTION number_next(jvxSize* num) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_next(jvxSize idx, IjvxConnectionIterator** next) override;

	jvxErrorType iterator_chain_x(IjvxConnectionIterator** it);
	jvxErrorType JVX_CALLINGCONVENTION number_next_x(jvxSize* num);
	jvxErrorType JVX_CALLINGCONVENTION reference_next_x(jvxSize idx, IjvxConnectionIterator** next);

#include "codeFragments/simplify/jvxConnectorFactory_simplify.h"

// For all non-object interfaces, return object reference
#define JVX_OBJECT_ZERO_REFERENCE
#include "codeFragments/simplify/jvxInterfaceReference_simplify.h"
#undef JVX_OBJECT_ZERO_REFERENCE

#include "codeFragments/simplify/jvxDataConnections_simplify.h"
};

#endif
