#ifndef _CJVXHOSTJSONCOMMANDSSHOW_H__
#define _CJVXHOSTJSONCOMMANDSSHOW_H__

#include "CjvxHostJsonCommandsProperties.h"

class CjvxHostJsonCommandsShow: public CjvxHostJsonCommandsProperties, public CjvxHostJsonProcessCommand
{
public:
	CjvxHostJsonCommandsConfigShow config_show;
protected:
	IjvxHost*& hHost;
public:
	CjvxHostJsonCommandsShow(IjvxHost*& hHostRefArg) : CjvxHostJsonCommandsProperties(config_show), hHost(hHostRefArg)
	{
		
	};

	virtual jvxErrorType process_command_abstraction(
		const oneDrivehostCommand& dh_command, const std::vector<std::string>& args,
		const std::string& addArg,
		jvxSize off, CjvxJsonElementList& jsec, jvxBool* systemUpdate) override;

	jvxErrorType create_active_component_one_type(jvxComponentType tpelm, CjvxJsonArray& jelm, jvxBool& anyElementAdded);
	jvxErrorType produce_component_core(CjvxJsonElementList& jsec);

	jvxErrorType show_components(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jsecg);
	jvxErrorType show_connections(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jsecg);
	jvxErrorType show_connection_rules(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jsecg);
	jvxErrorType show_system_compact(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jsec);
	jvxErrorType show_sequencer(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmlst);
	jvxErrorType show_version(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jsec);
	jvxErrorType output_one_sequence(IjvxSequencer* sequencer, jvxSize i, jvxBool showRunQueue, jvxBool showLeaveQueue, jvxSize idSS, CjvxJsonElementList& lstelmr);
	jvxErrorType output_one_step_sequence(IjvxSequencer* sequencer, jvxSize i, jvxSize j, jvxSequencerQueueType qTp, CjvxJsonElementList& fillme);
	jvxErrorType output_one_process(IjvxDataConnections* connections, jvxSize idx, const std::string& specName, CjvxJsonElementList& lstelmr,
		std::string& errTxt, jvxDrivehostConnectionShow showmode);
	jvxErrorType output_one_connection_rule(IjvxDataConnections* connections, jvxSize idx, const std::string& specName, CjvxJsonElementList& lstelmr,
		std::string& errTxt);
	
	jvxErrorType output_connections_core(
		CjvxJsonElementList& jsec,
		jvxDrivehostConnectionShow showmode = JVX_DRIVEHOST_CONNECTION_SHOW_NORMAL,
		jvxBool arg2set = false, jvxSize specId = JVX_SIZE_UNSELECTED, 
		const std::string& specName = "");

	jvxErrorType output_one_process_details(IjvxDataConnectionProcess* oneProcess, CjvxJsonElement& lstelmr);
	jvxErrorType output_one_rule_details(IjvxDataConnectionRule* oneRule, CjvxJsonElement& elmr);
	jvxErrorType output_sequencer_status(CjvxJsonElementList& jelmlst);

	jvxErrorType show_single_component(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, const std::string& addArg, jvxSize off, CjvxJsonElementList& jelmret);
	jvxErrorType show_hosttype_handler(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret);
	jvxErrorType show_config_line(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmlstret);
	jvxErrorType show_system(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jsec);
	jvxErrorType show_alive(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmlst_ret);

	virtual jvxErrorType show_current_config(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmlst_ret);
	virtual jvxErrorType act_edit_config(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jsec);

	jvxErrorType show_device_capabilities(CjvxJsonElement& jelm_result, const jvxDeviceCapabilities& caps);

	virtual jvxErrorType requestReferencePropertiesObject(jvx_propertyReferenceTriple& theTriple, const jvxComponentIdentification& tp) override;
	virtual jvxErrorType returnReferencePropertiesObject(jvx_propertyReferenceTriple& theTriple, const jvxComponentIdentification& tp) override;
};

#endif
