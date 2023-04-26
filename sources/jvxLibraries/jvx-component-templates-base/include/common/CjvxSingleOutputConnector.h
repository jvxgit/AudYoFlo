#ifndef __CJVXSINGLEOUTPUTCONNECTOR_H__
#define __CJVXSINGLEOUTPUTCONNECTOR_H__

#include "jvx.h"
#include "common/CjvxOutputConnectorLink.h"
#include "common/CjvxSingleConnectorReport.h"
#include "common/CjvxNegotiate.h"

class CjvxSingleOutputConnector: public IjvxOutputConnector, public CjvxOutputConnectorLink
{
protected:

	CjvxSingleConnector_report< CjvxSingleOutputConnector>* report = nullptr;
	CjvxNegotiate_output neg_output;
public:
	CjvxSingleOutputConnector();

	jvxErrorType activate(IjvxObject* theObj, IjvxConnectorFactory* conFac, const std::string& nm, CjvxSingleConnector_report<CjvxSingleOutputConnector>* reportArg);
	jvxErrorType deactivate();
	// =======================================================================================

	jvxErrorType updateFixedProcessingArgs(const jvxLinkDataDescriptor_con_params& params, jvxBool requestTestChain);

#define JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART
#define JVX_CONNECTOR_NOT_DERIVED_FROM_OBJECT
#define JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE nullptr
#define JVX_SUPPRESS_AUTO_READY_CHECK_ICON
#include "codeFragments/simplify/jvxOutputConnector_simplify.h"
#include "codeFragments/simplify/jvxConnectorCommon_simplify.h"
#undef JVX_INPUT_OUTPUT_CONNECTOR_SUPPRESS_AUTOSTART
#undef JVX_CONNECTOR_NOT_DERIVED_FROM_OBJECT
#undef JVX_INPUTOUTPUT_CONNECTOR_OBJECT_REFERENCE
#undef JVX_SUPPRESS_AUTO_READY_CHECK_ICON

};

#endif
