#ifndef _CJVXHOSTJSONCOMMANDSACTSHOW_H__
#define _CJVXHOSTJSONCOMMANDSACTSHOW_H__

#include "CjvxHostJsonCommandsShow.h"

class CjvxHostJsonCommandsActShow : 
	public CjvxHostJsonCommandsShow,
	public CjvxConnectionDropzone
{
protected:
	struct config_act_type
	{
		jvxBool silent_out = true;
	} config_act;

public:

	CjvxHostJsonCommandsActShow(IjvxHost*& hHostRefArg) :
		CjvxHostJsonCommandsShow(hHostRefArg) 
	{
	};

	jvxErrorType process_command_abstraction(
			const oneDrivehostCommand& dh_command, const std::vector<std::string>& args,
			const std::string& addArg, jvxSize off, CjvxJsonElementList& jsec, jvxBool* systemUpdate) override;

	jvxErrorType act_edit_config(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jsec);

	jvxErrorType act_edit_sequencer(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret);

	jvxErrorType act_edit_add_sequence(const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret);

	jvxErrorType act_edit_remove_sequence(const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret);

	jvxErrorType act_edit_activate_sequence(const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret);


	jvxErrorType act_edit_deactivate_sequence(const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret);

	jvxErrorType act_edit_add_step_sequence(const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret);


	jvxErrorType act_edit_modify_step_sequence(const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret);


	jvxErrorType act_edit_remove_step_sequence(const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret);


	jvxErrorType act_edit_switch_step_sequence(const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret);


	jvxErrorType act_single_component(const oneDrivehostCommand& dh_command, const std::vector<std::string>& args, 
		const std::string& addArg, jvxSize off, CjvxJsonElementList& jelmret, jvxBool* systemUpdate);

	jvxErrorType act_edit_connection(const oneDrivehostCommand& dh_command,
			const std::vector<std::string>& args, jvxSize off,
			CjvxJsonElementList& jelmret);

	jvxErrorType act_edit_dropzone(const oneDrivehostCommand& dh_command,
			const std::vector<std::string>& args, jvxSize off,
			CjvxJsonElementList& jelmret);

	jvxErrorType act_add_master_dz(const std::string& arg2, const std::string& arg3, const std::string& arg4,
			IjvxDataConnections* connections, std::string& errTxt);

	jvxErrorType act_add_ocon_dz(const std::string& arg2, const std::string& arg3, const std::string& arg4,
			IjvxDataConnections* connections, std::string& errTxt);

	jvxErrorType act_create_bridge_dz(const std::string& arg2, const std::string& arg3,
			const std::string& arg4, std::string& errTxt);

	jvxErrorType act_disconnect_bridge_dz(const std::string& arg2, std::string& errTxt);

	jvxErrorType act_process_connect_dz(const std::string& arg2, const std::string& arg3, IjvxDataConnections* connections, std::string& errTxt);

	jvxErrorType act_connection_create_connect_dz(const std::string& arg2, const std::string& arg3,
			const std::string& arg4, const std::string& arg5, IjvxDataConnections* connections, std::string& errTxt);

	jvxErrorType act_add_icon_dz(const std::string& arg2, const std::string& arg3, const std::string& arg4,
			IjvxDataConnections* connections, std::string& errTxt);

	jvxErrorType act_get_property_component(const oneDrivehostCommand& dh_command, 
		const std::vector<std::string>& args, jvxSize off, CjvxJsonElementList& jelmret,
		jvxBool content_only = false, jvxBool compact = false);

	jvxErrorType act_set_property_component(const oneDrivehostCommand& dh_command, const std::string& identificationTarget,
			const std::string& loadTarget, const std::string& offsetStart, CjvxJsonElementList& jelmret, jvxBool reportSet = false);

	jvxErrorType act_state_switch_component(const oneDrivehostCommand& dh_command,
			const std::string& identificationComponent, jvxStateSwitch theSwitch, CjvxJsonElementList& jelmret,
			const std::string& argSlotid, const std::string& argSlotSubid, jvxBool* systemUpdate);

	virtual jvxErrorType act_start_sequencer(CjvxJsonElementList& jelmret) 
	{
		return JVX_ERROR_UNSUPPORTED;
	};
	virtual jvxErrorType act_stop_sequencer(CjvxJsonElementList& jelmret, jvxBool full_stop) 
	{
		return JVX_ERROR_UNSUPPORTED;
	};
};

#endif
