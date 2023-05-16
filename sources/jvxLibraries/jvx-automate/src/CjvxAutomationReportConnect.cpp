#include "CjvxAutomationReportConnect.h"

CjvxAutomationReportConnect::CjvxAutomationReportConnect(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxSimpleComponent*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);
	_common_set.theInterfaceFactory = static_cast<IjvxInterfaceFactory*>(this);

	tp_rep_activate_filter.clear();
	tp_rep_activate_filter.push_back(JVX_COMPONENT_AUDIO_TECHNOLOGY);
}

CjvxAutomationReportConnect::~CjvxAutomationReportConnect()
{
}

jvxErrorType
CjvxAutomationReportConnect::initialize(IjvxHiddenInterface* hostRef)
{
	return (CjvxObject::_initialize(hostRef));
}

jvxErrorType 
CjvxAutomationReportConnect::activate()
{
	jvxErrorType res = _activate();
	if (res == JVX_NO_ERROR)
	{
		JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
		log << "Activated <" << nmComponent << ">." << std::endl;
		JVX_STOP_LOCK_LOG;

		IjvxReport* theReport = nullptr;
		_common_set_min.theHostRef->request_hidden_interface(JVX_INTERFACE_REPORT, reinterpret_cast<jvxHandle**>(&theReport));
		IjvxObject* obj = nullptr;
		_common_set_min.theHostRef->object_hidden_interface(&obj);
		IjvxHost* hostRef = castFromObject<IjvxHost>(obj);
		this->set_system_references(theReport, hostRef);
	}
	return res;
}

jvxErrorType 
CjvxAutomationReportConnect::deactivate()
{
	this->set_system_references(nullptr, nullptr);

	JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
	log << "Deactivating <" << nmComponent << ">." << std::endl;
	JVX_STOP_LOCK_LOG;
		
	return(_deactivate());
}

// ======================================================================================

jvxErrorType
CjvxAutomationReportConnect::handle_report_ident(jvxReportCommandRequest req,
	jvxComponentIdentification tp, const std::string& ident,
	CjvxAutomationReport::callSpecificParameters* params)
{
	switch (req)
	{
	case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_BORN_SUBDEVICE:

		JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
		log << __FUNCTION__ << "Sub device was born!" << std::endl;
		JVX_STOP_LOCK_LOG;
		break;

	case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_DIED_SUBDEVICE:

		JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
		log << __FUNCTION__ << "Sub device has died!" << std::endl;		
		JVX_STOP_LOCK_LOG;
		break;

	default:
		JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
		log << __FUNCTION__ << "Another event was reported!" << std::endl;
		JVX_STOP_LOCK_LOG;
	}

	return CjvxAutomationReport::handle_report_ident(req, tp, ident, params);
}

jvxErrorType
CjvxAutomationReportConnect::handle_report_uid(jvxReportCommandRequest req, 
	jvxComponentIdentification tp, jvxSize uid, CjvxAutomationReport::callSpecificParameters* params)
{
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE_CLASS(fdb);
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE_CLASS_INIT(fdb);
	jvxBool addBeforeStart = true;
	IjvxDataConnections* con = nullptr;
	switch (req)
	{
	case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_CONNECTED:
	case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_TEST_SUCCESS:

		JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
		log << __FUNCTION__ << " - Report process connect for Uid <" << uid << ">." << std::endl;
		JVX_STOP_LOCK_LOG;	

		con = reqInterface<IjvxDataConnections>(refHostRef);
		if (con)
		{
			jvxSize i;
			IjvxDataConnectionProcess* proc = nullptr;
			con->reference_connection_process_uid(uid, &proc);
			if (proc)
			{
				jvxApiString dstr;
				jvxApiString modName;
				jvxApiString lCtxt;
				jvxApiString description;
				jvxComponentIdentification tpCp;
				proc->description(&dstr);

#ifdef JVX_AUTOMATION_VERBOSE
				std::cout << "==> Process <" << dstr.std_str() << std::endl;
#endif
				CayfAutomationModules::CayfAutomationModuleHandler::try_associate_process(uid, dstr.std_str());

				IjvxConnectionIterator* it = nullptr;
				proc->iterator_chain(&it);
				while (1)
				{
					if (it == nullptr)
					{
						break;
					}
					else
					{
						it->reference_component(&tpCp, &modName, &description, &lCtxt);

#ifdef JVX_AUTOMATION_VERBOSE
						std::cout << " - Component " << jvxComponentIdentification_txt(tpCp) << " -- " << modName.std_str() << " -- " << lCtxt.std_str() << std::endl;
#endif
						CayfAutomationModules::CayfAutomationModuleHandler::try_adapt_submodules(uid, modName.std_str(), description.std_str(), tpCp, req);

						jvxSize nn = 0;
						it->number_next(&nn);
						if (nn == 0)
						{
							// Last element in chain
							it = nullptr;
						}
						else if (nn == 1)
						{
							it->reference_next(0, &it);
						}
						else
						{
							for (i = 0; i < nn; i++)
							{
								IjvxConnectionIterator* itn = nullptr;
								it->reference_next(i, &itn);
							}
						}
					}
				}

				// Run the auto test only on process connected, otherwise we will produce an infinite loop!!
				if (req == jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_CONNECTED)
				{
					// Test this connection
					jvxErrorType resTest = proc->test_chain(true JVX_CONNECTION_FEEDBACK_CALL_A(fdb));

					// Indicate that the "test" has been done and is not required anymore
					proc->set_test_on_connect(false);
				}

				con->return_reference_connection_process(proc);
			}
		}
		break;
	case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_PROCESS_TO_BE_DISCONNECTED:

		JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
		log << __FUNCTION__ << " - Report process to be disconnected Uid <" << uid << ">." << std::endl;
		JVX_STOP_LOCK_LOG;

		CayfAutomationModules::CayfAutomationModuleHandler::try_deassociate_process(uid);

		break;
	default:

		JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
		log << __FUNCTION__ << " - Report process Uid <" << uid << ">, request <" << (int)req << ">." << std::endl;
		JVX_STOP_LOCK_LOG;
		break;
	}
	return CjvxAutomationReport::handle_report_uid(req, tp, uid, params);
}

jvxErrorType
CjvxAutomationReportConnect::handle_report_ss(
	jvxReportCommandRequest req,
	jvxComponentIdentification tp,
	jvxStateSwitch ss,
	CjvxAutomationReport::callSpecificParameters* params)
{
	jvxApiString astr;
	jvxErrorType res = JVX_NO_ERROR;

	JVX_START_LOCK_LOG(jvxLogLevel::JVX_LOGLEVEL_3_DEBUG_OPERATION_WITH_LOW_DEGREE_OUTPUT);
	log << __FUNCTION__ << " - Report state switch component <" << jvxComponentIdentification_txt(tp) << ">, state switch <" << jvxStateSwitch_txt(ss) << ">." << std::endl;
	JVX_STOP_LOCK_LOG;

	return res;
}

jvxErrorType 
CjvxAutomationReportConnect::request_command(const CjvxReportCommandRequest& request)
{
	jvxErrorType res = CjvxAutomationReport::request_command(request);
	if (res == JVX_NO_ERROR)
	{
		switch (request.request())
		{
		case jvxReportCommandRequest::JVX_REPORT_COMMAND_REQUEST_REPORT_CONFIGURATION_COMPLETE:
			
			break;
		}
	}
	return res;
}

jvxErrorType
CjvxAutomationReportConnect::request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch (tp)
	{
	case JVX_INTERFACE_AUTO_DATA_CONNECT:

		*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxAutoDataConnect*>(this));
		break;
	case JVX_INTERFACE_REPORT_SYSTEM:

		*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxReportSystem*>(this));
		break;

	default:
		res = _request_hidden_interface(tp, hdl);
	}
	return(res);
}

// ======================================================================================

jvxErrorType 
CjvxAutomationReportConnect::report_connection_factory_to_be_added(
	jvxComponentIdentification tp_activated,
	IjvxConnectorFactory* toadd
)
{
	CayfAutomationModules::CayfAutomationModuleHandler::try_activate_submodules(tp_activated);
	// Activate the required sub modules. If the type is not matched, function return JVX_ERROR_INVALID_SETTING
	// This function decides internally if the submodule is activated, therefore function fail is very common

	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAutomationReportConnect::report_connection_factory_removed(
	jvxComponentIdentification tp_deactivated,
	IjvxConnectorFactory* toremove) 
{
	CayfAutomationModules::CayfAutomationModuleHandler::try_deactivate_submodules(tp_deactivated);
	// Deactivate the required sub modules. If the type is not matched, function return JVX_ERROR_INVALID_SETTING
	// This function decides internally if the submodule is activated, therefore function fail is very common

	return JVX_NO_ERROR;
}

// ======================================================================================

jvxErrorType
CjvxAutomationReportConnect::return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	jvxErrorType res = JVX_NO_ERROR;

	switch (tp)
	{
	case JVX_INTERFACE_AUTO_DATA_CONNECT:
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxAutoDataConnect*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	case JVX_INTERFACE_REPORT_SYSTEM:
		if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxReportSystem*>(this)))
		{
			res = JVX_NO_ERROR;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;

	default:
		res = _return_hidden_interface(tp, hdl);
	}
	return(res);
}

jvxErrorType
CjvxAutomationReportConnect::object_hidden_interface(IjvxObject** objRef)
{
	if (objRef)
	{
		*objRef = static_cast<IjvxObject*>(this);
	}
	return JVX_NO_ERROR;
}
