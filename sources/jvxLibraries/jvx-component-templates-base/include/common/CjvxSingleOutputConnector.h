#ifndef __CJVXSINGLEOUTPUTCONNECTOR_H__
#define __CJVXSINGLEOUTPUTCONNECTOR_H__

#include "jvx.h"
#include "common/CjvxOutputConnectorLink.h"

class CjvxSingleOutputConnector: public IjvxOutputConnector, public CjvxOutputConnectorLink
{
public:
	CjvxSingleOutputConnector();

	// =======================================================================================

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
