#include "CayfATTypical.h"

CayfATTypical::CayfATTypical(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxAutomationReportConnect(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL), CjvxProperties(module_name, *this)
{
	nmComponent = "CayfATTypical";
}

CayfATTypical::~CayfATTypical()
{
}

// ==================================================================================================

jvxErrorType 
CayfATTypical::select(IjvxObject* owner)
{
	jvxErrorType res = CjvxAutomationReportConnect::select(owner);
	if (res == JVX_NO_ERROR)
	{
		// ===================================================================================================

		genATTypical::init_all();
		genATTypical::allocate_all();
		genATTypical::register_all(this);
	}
	return res;
}

jvxErrorType 
CayfATTypical::unselect()
{
	jvxErrorType res = CjvxAutomationReportConnect::_pre_check_unselect();
	if (res == JVX_NO_ERROR)
	{
		genATTypical::unregister_all(this);
		genATTypical::deallocate_all();

		CjvxAutomationReportConnect::unselect();
	}
	return res;
}

// ==================================================================================================

jvxErrorType 
CayfATTypical::activate()
{
	jvxErrorType res = CjvxAutomationReportConnect::activate();
	return res;
}

jvxErrorType
CayfATTypical::deactivate()
{
	jvxErrorType res = CjvxAutomationReportConnect::_pre_check_deactivate();
	return res;
}

// ==========================================================================================================

jvxErrorType
CayfATTypical::handle_report_ss(
	jvxReportCommandRequest req,
	jvxComponentIdentification tp,
	jvxStateSwitch ss,
	CjvxAutomationReport::callSpecificParameters* params)
{	
	jvxApiString astr;
	jvxErrorType res = JVX_NO_ERROR;
	if (tp == jvxComponentIdentification(JVX_COMPONENT_AUDIO_TECHNOLOGY, 0, 0))
	{
		IjvxObject* theObj = nullptr;
		this->refHostRef->request_object_selected_component(tp, &theObj);
		if (theObj)
		{
			IjvxProperties* theProp = reqInterfaceObj<IjvxProperties>(theObj);
			if (theProp)
			{
				std::string searchToken = "no-specific";
#ifdef JVX_OS_WINDOWS
				if (genATTypical::config_audio.primary_audio_tech_lowlevel.value)
				{
					searchToken = "*ASIO*";
				}
				else
				{
					searchToken = "*Windows*";
				}
#elif defined JVX_OS_LINUX
				if (genATTypical::config_audio.primary_audio_tech_lowlevel.value)
				{
					searchToken = "*ALSA*";
				}
				else
				{
					searchToken = "*Pipewire*";
				}
#elif defined JVX_OS_MACOSX
				searchToken = "*CoreAudio*";
#elif defined JVX_OS_ANDROID
				searchToken = "*Android*";
#endif

				res = prepareAudioTechnologyOnStateSwitch(ss, theProp, searchToken);
				retInterfaceObj<IjvxProperties>(theObj, theProp);
			}
			this->refHostRef->return_object_selected_component(tp, theObj);
		}
	}
	return CjvxAutomationReportConnect::handle_report_ss(req, tp, ss, params);
}

jvxErrorType
CayfATTypical::report_connection_factory_to_be_added(
	jvxComponentIdentification tp_activated,
	IjvxConnectorFactory* add)
{
	return CjvxAutomationReportConnect::report_connection_factory_to_be_added(
		tp_activated, add);
};

jvxErrorType
CayfATTypical::report_connection_factory_removed(
	jvxComponentIdentification tp_deactivated,
	IjvxConnectorFactory* toremove)
{
	return CjvxAutomationReportConnect::report_connection_factory_removed(
		tp_deactivated, toremove);
}

jvxErrorType
CayfATTypical::request_command(const CjvxReportCommandRequest& request)
{
	jvxErrorType res = CjvxAutomationReport::request_command(request);
	const CjvxReportCommandRequest_uid* elm = nullptr;
	jvxSize uid = JVX_SIZE_UNSELECTED;
	if (res == JVX_NO_ERROR)
	{
		switch (request.request())
		{
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_CONFIGURATION_COMPLETE:

			// Connect the chains in case some connections could not be achieved during configuration
			CayfAutomationModules::CayfAutomationModuleHandler::report_configuration_done();
			break;

		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_TO_BE_DISCONNECTED:

			// Connect the chains in case some connections could not be achieved during configuration
			// CayfAutomationModules::CayfAutomationModuleHandler::report_configuration_done();
			
			elm = castCommandRequest<CjvxReportCommandRequest_uid>(request);
			elm->uid(&uid);
			CayfAutomationModules::CayfAutomationModuleHandler::report_to_be_disconnected(uid);
			break;
		}
	}
	return res;
}

jvxErrorType 
CayfATTypical::request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)
{
	jvxErrorType res = JVX_NO_ERROR;
	switch (tp)
	{
	case JVX_INTERFACE_PROPERTIES:

		*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxProperties*>(this));
		break;

	case JVX_INTERFACE_CONFIGURATION:

		*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxConfiguration*>(this));
		break;

	default:
		res = CjvxAutomationReportConnect::request_hidden_interface(tp, hdl);
	}
	return res;
}

jvxErrorType 
CayfATTypical::return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch (tp)
	{
	case JVX_INTERFACE_PROPERTIES:
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxProperties*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	case JVX_INTERFACE_CONFIGURATION:
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxConfiguration*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	default:
		res = CjvxAutomationReportConnect::return_hidden_interface(tp, hdl);
	}
	return res;
}

jvxErrorType 
CayfATTypical::put_configuration(jvxCallManagerConfiguration* callMan, IjvxConfigProcessor* processor,
	jvxHandle* sectionToContainAllSubsectionsForMe, const char* filename, jvxInt32 lineno)
{
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		genATTypical::put_configuration_all(callMan, processor, sectionToContainAllSubsectionsForMe);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CayfATTypical::get_configuration(jvxCallManagerConfiguration* callMan,
	IjvxConfigProcessor* processor, jvxHandle* sectionWhereToAddAllSubsections)
{
	if (_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		genATTypical::get_configuration_all(callMan, processor, sectionWhereToAddAllSubsections);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CayfATTypical::prepareAudioTechnologyOnStateSwitch(jvxStateSwitch ss, IjvxProperties* theProp, const std::string& lookforAudioTechWC)
{
	jvxSelectionList sel;
	jPROSL raw(sel);
	jPAD ident;
	jvxErrorType res = JVX_NO_ERROR;
	jvxCallManagerProperties callGate;
	switch (ss)
	{
	case JVX_STATE_SWITCH_SELECT:
		ident.reset("/JVX_GENW_TECHNOLOGIES");
		res = theProp->get_property(callGate, raw, ident);
		if (res == JVX_NO_ERROR)
		{
			jvx_bitFClear(sel.bitFieldSelected());
			for (int i = 0; i < sel.strList.ll(); i++)
			{
				if (jvx_compareStringsWildcard(lookforAudioTechWC, sel.strList.std_str_at(i)))
				{
					jvx_bitZSet(sel.bitFieldSelected(), i);
					break;
				}
			}
			if (jvx_bitFieldValue32(sel.bitFieldSelected()))
			{
				theProp->set_property(callGate, raw, ident);
			}
		}
		break;

	}
	return res;
}
