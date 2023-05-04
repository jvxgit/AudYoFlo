#ifndef __CJVXSINGLEOUTPUTCONNECTOR_H__
#define __CJVXSINGLEOUTPUTCONNECTOR_H__

#include "jvx.h"
#include "common/CjvxOutputConnectorLink.h"
#include "common/CjvxSingleConnectorReport.h"
#include "common/CjvxSingleConnectorCommon.h"
#include "common/CjvxNegotiate.h"

class CjvxSingleOutputConnector;

class CjvxSingleOutputTriggerConnector : public CjvxSingleTriggerConnector<IjvxTriggerInputConnector, IjvxTriggerOutputConnector>
{
public:
	CjvxSingleOutputConnector* bwdRef = nullptr;
public:
	virtual jvxErrorType trigger(jvxTriggerConnectorPurpose purp, jvxHandle* data JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
};

class CjvxSingleOutputConnector : public IjvxOutputConnector, public IjvxConnectionIterator, public CjvxConnector<CjvxOutputConnectorLink, CjvxSingleOutputTriggerConnector>
{
	friend class CjvxSingleOutputTriggerConnector;

protected:

	CjvxSingleConnector_report< CjvxSingleOutputConnector>* report = nullptr;
	CjvxNegotiate_output neg_output;

public:
	CjvxSingleOutputConnector(jvxBool withTriggerConnectorArg);
	~CjvxSingleOutputConnector();

	jvxErrorType activate(IjvxObject* theObj, IjvxConnectorFactory* conFac, const std::string& nm, 
		CjvxSingleConnector_report<CjvxSingleOutputConnector>* reportArg, jvxSize conIdArg);
	jvxErrorType deactivate();

	// =======================================================================================
	// =======================================================================================

	jvxErrorType updateFixedProcessingArgs(const jvxLinkDataDescriptor_con_params& params, jvxBool requestTestChain);

	virtual jvxErrorType request_trigger_itcon(IjvxTriggerInputConnector** otcon) override;
	virtual jvxErrorType return_trigger_itcon(IjvxTriggerInputConnector* otcon) override;

	jvxErrorType number_next(jvxSize*) override;
	jvxErrorType reference_next(jvxSize, IjvxConnectionIterator**) override;
	jvxErrorType reference_component(jvxComponentIdentification*, jvxApiString*, jvxApiString*, jvxApiString*)override;

#define JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART
#define JVX_CONNECTOR_NOT_DERIVED_FROM_OBJECT
#define JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE nullptr
#define JVX_SUPPRESS_AUTO_READY_CHECK_ICON
#define JVX_INPUT_OUTPUT_SUPPRESS_TRIGGER_CONNECTOR
#include "codeFragments/simplify/jvxOutputConnector_simplify.h"
#include "codeFragments/simplify/jvxConnectorCommon_simplify.h"
#undef JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART
#undef JVX_CONNECTOR_NOT_DERIVED_FROM_OBJECT
#undef JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE
#undef JVX_SUPPRESS_AUTO_READY_CHECK_ICON
#undef JVX_INPUT_OUTPUT_SUPPRESS_TRIGGER_CONNECTOR
};

class CjvxSingleOutputConnectorMulti : public CjvxConnectorMulti< IjvxOutputConnector, CjvxSingleOutputConnector>
{
public:
	CjvxSingleOutputConnectorMulti(jvxBool withTriggerConnectorArg) :
		CjvxConnectorMulti< IjvxOutputConnector, CjvxSingleOutputConnector>(withTriggerConnectorArg) {};

	virtual jvxErrorType JVX_CALLINGCONVENTION select_connect_ocon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common, IjvxOutputConnector** replace_connector) override;
	virtual jvxErrorType JVX_CALLINGCONVENTION unselect_connect_ocon(IjvxConnectorBridge* obj,
		IjvxOutputConnector* replace_connector) override;
};


#endif
