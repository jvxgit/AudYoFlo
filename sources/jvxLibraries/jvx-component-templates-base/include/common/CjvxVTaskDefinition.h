#ifndef __CJVXVTASKDEFINITION_H__
#define __CJVXVTASKDEFINITION_H__

#include "jvx.h"
#include "common/jvxTypedefsVTask.h"

class CjvxVTaskDefinition: public IjvxInputConnectorMulti
{
public:
	std::string name;
	std::vector<std::string> descriptor_in;
	std::vector<std::string> descriptor_out;

	jvxBool withMaster;
	jvxSize numMaxTasks;
	jvxSize numCurrentTasks;
	jvxSize idOffThisTasks;
	
	/*
	std::list<jvxOneConnectorTask> connectedTasks;
	*/
	std::list<oneConnectorVTask_con<CjvxInputConnectorVtask> > icon;
	std::list<oneConnectorVTask_con<CjvxOutputConnectorVtask> >  ocon;
	IjvxInputOutputConnectorVtask* reportSelect;

	std::map<IjvxDataConnectionCommon*, jvxOneConnectorTaskRuntime> activeRuntimeTasks;

	CjvxVTaskDefinition();

	virtual jvxSize number_connected_icon(jvxConnectorSelectType sel = jvxConnectorSelectType::JVX_CONNECTOR_SELECT_CONNECTABLE, jvxHandle* ctxt = nullptr) override;
	virtual IjvxInputConnector* reference_connected_icon(jvxSize idx, jvxConnectorSelectType sel = jvxConnectorSelectType::JVX_CONNECTOR_SELECT_CONNECTABLE, jvxHandle* ctxt = nullptr) override;
	virtual jvxErrorType return_connected_icon(IjvxInputConnector* icon, jvxConnectorSelectType sel = jvxConnectorSelectType::JVX_CONNECTOR_SELECT_CONNECTABLE, jvxHandle* ctxt = nullptr) override;
} ;

#endif

