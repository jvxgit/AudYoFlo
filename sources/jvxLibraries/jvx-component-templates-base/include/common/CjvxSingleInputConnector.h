#ifndef __CJVXSINGLEINPUTCONNECTOR_H__
#define __CJVXSINGLEINPUTCONNECTOR_H__

#include "jvx.h"
#include "common/CjvxInputConnectorLink.h"
#include "common/CjvxSingleConnectorReport.h"
#include "common/CjvxNegotiate.h"

class CjvxSingleInputConnector: public IjvxInputConnector, public CjvxInputConnectorLink
{
protected:

	CjvxSingleConnector_report<CjvxSingleInputConnector>* report = nullptr;
	CjvxNegotiate_input neg_input;

public:
	CjvxSingleInputConnector();
	~CjvxSingleInputConnector();

	jvxErrorType activate(IjvxObject* theObj, IjvxConnectorFactory* conFac, const std::string& nm, CjvxSingleConnector_report<CjvxSingleInputConnector>* reportArg);
	jvxErrorType deactivate();

	jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	jvxErrorType start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;
	jvxErrorType stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))override;

	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	jvxErrorType number_next(jvxSize*) override;
	jvxErrorType reference_next(jvxSize, IjvxConnectionIterator**) override;
	jvxErrorType reference_component(jvxComponentIdentification*, jvxApiString*, jvxApiString*)override;

	jvxErrorType updateFixedProcessingArgs(const jvxLinkDataDescriptor_con_params& params, jvxBool requestTestChain);

	// Request for a PUSH from the connected async connection!!
	jvxErrorType trigger_get_data();

	// =======================================================================================

#define JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART
#define JVX_CONNECTOR_NOT_DERIVED_FROM_OBJECT
#define JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE nullptr
#define JVX_SUPPRESS_AUTO_READY_CHECK_ICON
#define JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON
#define JVX_INPUT_OUTPUT_SUPPRESS_START_STOP_TO
#define JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_BUFFERS_TO
#include "codeFragments/simplify/jvxInputConnector_simplify.h"
#include "codeFragments/simplify/jvxConnectorCommon_simplify.h"
#undef JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART
#undef JVX_CONNECTOR_NOT_DERIVED_FROM_OBJECT
#undef JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE
#undef JVX_SUPPRESS_AUTO_READY_CHECK_ICON
#undef JVX_CONNECTION_MASTER_SKIP_TEST_CONNECT_ICON
#undef JVX_INPUT_OUTPUT_SUPPRESS_START_STOP_TO
#undef JVX_INPUT_OUTPUT_SUPPRESS_PROCESS_BUFFERS_TO

};

class CjvxSingleInputConnectorMulti: public CjvxSingleInputConnector
{
public:
	jvxSize acceptNumberConnectors = 1;
	jvxSize numConnectorsInUse = 0;

	std::map<IjvxInputConnector*, CjvxSingleInputConnector*> allocatedConnectors;

	CjvxSingleInputConnectorMulti();
	~CjvxSingleInputConnectorMulti();

	virtual jvxErrorType JVX_CALLINGCONVENTION select_connect_icon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common, IjvxInputConnector** replace_connector) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect_connect_icon(IjvxConnectorBridge* obj,
		IjvxInputConnector* replace_connector) override;
};

#endif
