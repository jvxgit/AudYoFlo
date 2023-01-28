#ifndef __CJVXBARENODENIO_H__
#define __CJVXBARENODENIO_H__

#include "CjvxNodeBaseNio.h"

#if 0 // <- module not ready currently
struct jvxOneConnectorDefine
{
	jvxSize idCon;
	std::string nmConnector;
	IjvxConnectionMaster* assocMaster;

	jvxOneConnectorDefine()
	{
		idCon = JVX_SIZE_UNSELECTED;
		assocMaster = NULL;
	}
};

// Make this mode an audio node
class CjvxBareNodeNio : public CjvxNodeBaseNio, public CjvxCommonNio_report
{
protected:

	typedef struct
	{
		std::vector<jvxOneConnectorDefine> inputConnectors;
		std::vector<jvxOneConnectorDefine> outputConnectors;
	} jvxOneConnectorTask;

	typedef struct
	{
		std::string descriptor;
		jvxSize numVariableTasks;
		jvxSize offVariableTasks;
	} jvxOneVariableConnectorTaskDefinition;

	struct
	{
		std::vector<jvxOneConnectorTask> fixedTasks;
		
		std::vector<jvxOneVariableConnectorTaskDefinition> variableTasksDefines;
			//std::vector<jvxOneConnectorTask> variableTasks;
	} _common_set_node_nio;

public:
	JVX_CALLINGCONVENTION CjvxBareNodeNio(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxBareNodeNio();

	virtual jvxErrorType JVX_CALLINGCONVENTION activate()override;
	virtual jvxErrorType JVX_CALLINGCONVENTION deactivate()override;

#define JVX_INTERFACE_SUPPORT_DATAPROCESSOR_CONNECT
#define JVX_INTERFACE_SUPPORT_DATAPROCESSOR
#define JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#define JVX_INTERFACE_SUPPORT_PROPERTIES
#define JVX_INTERFACE_SUPPORT_CONFIGURATION
#define JVX_INTERFACE_SUPPORT_CONNECTOR_FACTORY
#include "codeFragments/simplify/jvxHiddenInterface_simplify.h"
#undef JVX_INTERFACE_SUPPORT_DATAPROCESSOR
#undef JVX_INTERFACE_SUPPORT_SEQUENCER_CONTROL
#undef JVX_INTERFACE_SUPPORT_PROPERTIES
#undef JVX_INTERFACE_SUPPORT_CONFIGURATION
#undef JVX_INTERFACE_SUPPORT_DATAPROCESSOR_CONNECT
#undef JVX_INTERFACE_SUPPORT_LINKDATA_SLAVE
};

#endif
#endif