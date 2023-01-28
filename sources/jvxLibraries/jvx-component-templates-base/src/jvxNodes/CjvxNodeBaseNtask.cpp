#include "jvxNodes/CjvxNodeBaseNtask.h"

jvxErrorType
CjvxNodeBaseNtask::activate()
{
	jvxErrorType res = _activate();
	jvxErrorType resL = JVX_NO_ERROR;

	if (res == JVX_NO_ERROR)
	{
		CjvxNode_genpcg::init_all();
		CjvxNode_genpcg::allocate_all();
		CjvxNode_genpcg::register_all(static_cast<CjvxProperties*>(this));
		//JVX_ACTIVATE_DEFAULT_ONE_IN_ONE_OUT_CONNECTORS(NULL, static_cast<IjvxObject*>(this), "default", NULL);
	}
	return(res);
};

jvxErrorType
CjvxNodeBaseNtask::deactivate()
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = _deactivate();
	if (res == JVX_NO_ERROR)
	{
		CjvxNode_genpcg::unregister_all(static_cast<CjvxProperties*>(this));
		CjvxNode_genpcg::deallocate_all();
	}
	return(res);
};

jvxErrorType
CjvxNodeBaseNtask::is_ready(jvxBool* suc, jvxApiString* reasonIfNot)
{
	std::string txt;
	jvxErrorType res = _is_ready(suc, reasonIfNot);
	jvxBool Iamready = true;
	jvxBool isAProblem = false;

	if (res == JVX_NO_ERROR)
	{
		// 
	}

	return(res);
};


jvxErrorType
CjvxNodeBaseNtask::put_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe,
	const char* filename,
	jvxInt32 lineno)
{
	jvxSize i;
	std::vector<std::string> warns;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		CjvxNode_genpcg::put_configuration_all(callConf, processor,
			sectionToContainAllSubsectionsForMe, &warns);
		for (i = 0; i < warns.size(); i++)
		{
			_report_text_message(warns[i].c_str(), JVX_REPORT_PRIORITY_WARNING);
		}
	}
	return JVX_NO_ERROR;
}
jvxErrorType
CjvxNodeBaseNtask::get_configuration(jvxCallManagerConfiguration* callConf, IjvxConfigProcessor* processor,
	jvxHandle* sectionWhereToAddAllSubsections)
{
	CjvxNode_genpcg::get_configuration_all(callConf, processor, sectionWhereToAddAllSubsections);

	return JVX_NO_ERROR;
}

