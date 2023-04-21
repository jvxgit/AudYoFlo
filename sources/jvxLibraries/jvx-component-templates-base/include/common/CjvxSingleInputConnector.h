#ifndef __CJVXSINGLEINPUTCONNECTOR_H__
#define __CJVXSINGLEINPUTCONNECTOR_H__

#include "jvx.h"
#include "common/CjvxInputConnectorLink.h"

class CjvxSingleInputConnector;

JVX_INTERFACE CjvxSingleInputConnector_report
{
public:
	virtual ~CjvxSingleInputConnector_report() {};

	jvxErrorType report_selected_connector(CjvxSingleInputConnector* iconn);
	jvxErrorType report_unselected_connector(CjvxSingleInputConnector* iconn);
};

class CjvxSingleInputConnector: public IjvxInputConnector, public CjvxInputConnectorLink
{
protected:

	CjvxSingleInputConnector_report* report = nullptr;

public:
	CjvxSingleInputConnector();

	jvxErrorType activate(IjvxObject* theObj, IjvxConnectorFactory* conFac, const std::string& nm, CjvxSingleInputConnector_report* reportArg);
	jvxErrorType deactivate();

	jvxErrorType prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType number_next(jvxSize*) override;
	jvxErrorType reference_next(jvxSize, IjvxConnectionIterator**) override;
	jvxErrorType reference_component(jvxComponentIdentification*, jvxApiString*, jvxApiString*)override;

	// =======================================================================================

#define JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART
#define JVX_CONNECTOR_NOT_DERIVED_FROM_OBJECT
#define JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE nullptr
#define JVX_SUPPRESS_AUTO_READY_CHECK_ICON
#include "codeFragments/simplify/jvxInputConnector_simplify.h"
#include "codeFragments/simplify/jvxConnectorCommon_simplify.h"
#undef JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART
#undef JVX_CONNECTOR_NOT_DERIVED_FROM_OBJECT
#undef JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE
#undef JVX_SUPPRESS_AUTO_READY_CHECK_ICON

};

#endif
