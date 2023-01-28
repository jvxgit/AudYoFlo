#ifndef __CJVXHANDLECOMPONENTAUTOMATION_H__
#define __CJVXHANDLECOMPONENTAUTOMATION_H__

#include "jvx.h"

//#define JVX_AUTOMATION_VERBOSE

class CjvxAutomation: public IjvxReportSystem
{
public:
	class oneHandledComponent
	{
	public:
		class oneProcessComponent
		{
		public:
			jvxSize proc_id = JVX_SIZE_UNSELECTED;
			jvxSize task_id = JVX_SIZE_UNSELECTED;
			CjvxSequencerTask* theTask = nullptr;
		};
		jvxComponentIdentification cpTp;
		std::string ident;
		IjvxObject* component = nullptr;
		std::map<jvxSize, oneProcessComponent> ids_process_involded;
		jvxBool autoRemove = false;
		
	};
	//IjvxReportSystem* theSystemReport = nullptr;

	class callSpecificParameters
	{
	public:
		//! Additional parameter to add further specific options
		jvxBool addBeforeStart = true;
	};

	std::list<jvxComponentType> tp_rep_activate_filter;

protected:
	IjvxReportSystem* refSystemReport = nullptr;
	IjvxHost* refHostRef = nullptr;

	// HK, 31.01.2022: string as the key is not good: names occur multiple times!!
	// Ket us see if we can stick to the pointer approach!
	std::map<IjvxObject*, oneHandledComponent> handledComponents;
	//std::map<std::string, oneHandledComponent> handledComponents;
	//virtual jvxErrorType JVX_CALLINGCONVENTION request_command(IjvxReportCommandRequest* request) override;

public:

	CjvxAutomation();
	~CjvxAutomation();

	void set_system_references(IjvxReportSystem* reportArg, IjvxHost* refHostRefArg);

	// Interface to accepts messages from host system
	virtual jvxErrorType JVX_CALLINGCONVENTION request_command(const CjvxReportCommandRequest& request) override;

	//! Callback to report new component ("birth") and inactive component ("death"). Following action: activate	
	virtual jvxErrorType handle_report_ident(
		jvxReportCommandRequest req, 
		jvxComponentIdentification tp, 
		const std::string& ident,
		CjvxAutomation::callSpecificParameters* params);

	//! Report unique id to show that new data connection process has been established
	virtual jvxErrorType handle_report_uid(
		jvxReportCommandRequest req, 
		jvxComponentIdentification tp, 
		jvxSize uid, 
		callSpecificParameters* params);

	virtual jvxErrorType handle_report_ss(
		jvxReportCommandRequest req,
		jvxComponentIdentification tp,
		jvxStateSwitch ss,
		callSpecificParameters* params);

	void complete_dead_device_sequencer_immediate(std::map<jvxSize, CjvxAutomation::oneHandledComponent::oneProcessComponent>::iterator& elm);

	bool shall_be_handled(jvxComponentType tp);
};

#endif
