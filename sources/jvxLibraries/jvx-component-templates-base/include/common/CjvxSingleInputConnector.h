#ifndef __CJVXSINGLEINPUTCONNECTOR_H__
#define __CJVXSINGLEINPUTCONNECTOR_H__

#include "jvx.h"
#include "common/CjvxInputConnectorLink.h"
#include "common/CjvxSingleConnectorReport.h"
#include "common/CjvxSingleConnectorCommon.h"
#include "common/CjvxNegotiate.h"

class CjvxSingleInputConnector;

// ==================================================================================

class CjvxSingleInputTriggerConnector : public CjvxSingleTriggerConnector<IjvxTriggerOutputConnector, IjvxTriggerInputConnector>
{
public:
	CjvxSingleInputConnector* bwdRef = nullptr;
public:
	virtual jvxErrorType trigger(jvxTriggerConnectorPurpose purp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
};

class CjvxSingleInputConnector: public IjvxInputConnector, public CjvxConnector<CjvxInputConnectorLink, CjvxSingleInputTriggerConnector>
{
	friend class CjvxSingleInputTriggerConnector;

protected:

	CjvxSingleConnector_report<CjvxSingleInputConnector>* report = nullptr;
	CjvxNegotiate_input neg_input;

public:
	CjvxSingleInputConnector(jvxBool withTriggerConnectorArg);
	~CjvxSingleInputConnector();

	jvxErrorType activate(IjvxObject* theObj, IjvxConnectorFactory* conFac,
		const std::string& nm, 
		CjvxSingleConnector_report<CjvxSingleInputConnector>* reportArg, jvxSize idCon);
	jvxErrorType deactivate();

	jvxErrorType connect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	jvxErrorType disconnect_connect_icon(jvxLinkDataDescriptor* theData JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	jvxErrorType start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	jvxErrorType stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;


	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	jvxErrorType number_next(jvxSize*) override;
	jvxErrorType reference_next(jvxSize, IjvxConnectionIterator**) override;
	jvxErrorType reference_component(jvxComponentIdentification*, jvxApiString*, jvxApiString*, jvxApiString*)override;

	jvxErrorType updateFixedProcessingArgs(const jvxLinkDataDescriptor_con_params& params, jvxBool requestTestChain);

	// Request for a PUSH from the connected async connection!!
	jvxErrorType trigger_get_data();

	virtual jvxErrorType request_trigger_otcon(IjvxTriggerOutputConnector** otcon) override;
	virtual jvxErrorType return_trigger_otcon(IjvxTriggerOutputConnector* otcon) override;

	// =======================================================================================

#define JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART
#define JVX_CONNECTOR_NOT_DERIVED_FROM_OBJECT
#define JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE nullptr
#define JVX_SUPPRESS_AUTO_READY_CHECK_ICON
#define JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON
#define JVX_INPUT_OUTPUT_SUPPRESS_START_STOP_TO
#define JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_BUFFERS_TO
#define JVX_INPUT_OUTPUT_SUPPRESS_TRIGGER_CONNECTOR
#define JVX_CONNECTION_MASTER_SKIP_CONNECT_CONNECT_ICON
#define JVX_CONNECTION_MASTER_SKIP_DISCONNECT_CONNECT_ICON
#include "codeFragments/simplify/jvxInputConnector_simplify.h"
#include "codeFragments/simplify/jvxConnectorCommon_simplify.h"
#undef JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART
#undef JVX_CONNECTOR_NOT_DERIVED_FROM_OBJECT
#undef JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE
#undef JVX_SUPPRESS_AUTO_READY_CHECK_ICON
#undef JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON
#undef JVX_INPUT_OUTPUT_SUPPRESS_START_STOP_TO
#undef JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_BUFFERS_TO
#undef JVX_INPUT_OUTPUT_SUPPRESS_TRIGGER_CONNECTOR
#undef JVX_CONNECTION_MASTER_SKIP_CONNECT_CONNECT_ICON
#undef JVX_CONNECTION_MASTER_SKIP_DISCONNECT_CONNECT_ICON
};

// =======================================================================================

class CjvxSingleInputConnectorMulti: public CjvxConnectorMulti< IjvxInputConnector, CjvxSingleInputConnector>
{
public:

	CjvxSingleInputConnectorMulti(jvxBool withTriggerConnectorArg): 
		CjvxConnectorMulti< IjvxInputConnector, CjvxSingleInputConnector>(withTriggerConnectorArg) {};

	virtual jvxErrorType JVX_CALLINGCONVENTION select_connect_icon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common, IjvxInputConnector** replace_connector) override;

	virtual jvxErrorType JVX_CALLINGCONVENTION unselect_connect_icon(IjvxConnectorBridge* obj,
		IjvxInputConnector* replace_connector) override;
};



#endif
