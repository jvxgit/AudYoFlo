#include "CjvxAppHostBase.h"

// Includes for main host application
#ifdef JVX_HOST_USE_ONLY_STATIC_OBJECTS
#include "jvxHAppHost_nd.h"
#else
#include "jvxHAppHost.h"
#endif

extern "C"
{

	// Sequencer default steps
#define FUNC_CORE_PROT_DECLARE jvx_default_connection_rules_add
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_default_connection_rules_add_local
#define FUNC_CORE_PROT_ARGS IjvxDataConnections* theConn
#define FUNC_CORE_PROT_RETURNVAL jvxErrorType

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_default_connection_rules_add=jvx_default_connection_rules_add_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_default_connection_rules_add=_jvx_default_connection_rules_add_local")
#endif

#include "platform/jvx_platform_weak_defines.h"
	{
		std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
		// Default implementation does just nothing
		return JVX_DSP_ERROR_UNSUPPORTED;
	}

#undef FUNC_CORE_PROT_DECLARE
#undef FUNC_CORE_PROT_DECLARE_LOCAL
#undef FUNC_CORE_PROT_ARGS
#undef FUNC_CORE_PROT_RETURNVAL

	// Sequencer default steps
#define FUNC_CORE_PROT_DECLARE jvx_default_connections_add
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_default_connections_add_local
#define FUNC_CORE_PROT_ARGS IjvxDataConnections* theConn
#define FUNC_CORE_PROT_RETURNVAL jvxErrorType

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_default_connections_add=jvx_default_connections_add_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_default_connections_add=_jvx_default_connections_add_local")
#endif

#include "platform/jvx_platform_weak_defines.h"
	{
		std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
		// Default implementation does just nothing
		return JVX_DSP_ERROR_UNSUPPORTED;
	}

#undef FUNC_CORE_PROT_DECLARE
#undef FUNC_CORE_PROT_DECLARE_LOCAL
#undef FUNC_CORE_PROT_ARGS
#undef FUNC_CORE_PROT_RETURNVAL

	// Sequencer default steps
#define FUNC_CORE_PROT_DECLARE jvx_default_sequence_add
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_default_sequence_add_local
#define FUNC_CORE_PROT_ARGS IjvxSequencer* theSeq
#define FUNC_CORE_PROT_RETURNVAL jvxErrorType

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_default_sequence_add=jvx_default_sequence_add_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_default_sequence_add=_jvx_default_sequence_add_local")
#endif

#include "platform/jvx_platform_weak_defines.h"
	{
		std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
		// Default implementation does just nothing
		return JVX_DSP_ERROR_UNSUPPORTED;
	}

#undef FUNC_CORE_PROT_DECLARE
#undef FUNC_CORE_PROT_DECLARE_LOCAL
#undef FUNC_CORE_PROT_ARGS
#undef FUNC_CORE_PROT_RETURNVAL

}


JVX_APPHOST_CLASSNAME::JVX_APPHOST_CLASSNAME()
{
	confHostFeatures = NULL;
	onConnectionRulesStarted = false;
	onComponentsBeforeConfig = false;
	onComponentsConfigured = false;
	onSequencerStarted = false;
	onConnectionsStarted = false;
	boot_steps = static_cast<IjvxBootSteps*>(this);
}

JVX_APPHOST_CLASSNAME::~JVX_APPHOST_CLASSNAME()
{
}

void
JVX_APPHOST_CLASSNAME::resetReferences()
{
	JVX_APP_FACTORY_HOST_CLASSNAME::resetReferences();
	involvedHost.hHost = NULL;	
}

// =====================================================================================================
// INITIALIZE TERMINATE INITIALIZE TERMINATE INITIALIZE TERMINATE INITIALIZE TERMINATE INITIALIZE TERMINATE
// =====================================================================================================

jvxErrorType
JVX_APPHOST_CLASSNAME::openHostFactory(jvxApiString* errorMessage, jvxHandle* context)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxComponentIdentification cpType;
	bool multObj = false;

	// ======================================================
	// Initialize the host component
	// ======================================================
#ifdef JVX_HOST_USE_ONLY_STATIC_OBJECTS
	res = jvxHAppHost_nd_init(&involvedComponents.theHost.hobject, NULL);
#else
	res = jvxHAppHost_init(&involvedComponents.theHost.hobject, NULL);
#endif
	if (res != JVX_NO_ERROR)
	{
		if (errorMessage)
		{
			errorMessage->assign("Unable to open core host lib.");
		}
		return JVX_ERROR_INTERNAL;
	}

	if (verbose == true)
	{
		// Report progress
		report_text_output("Successfully openened host library", JVX_REPORT_PRIORITY_VERBOSE, NULL);
	}

	// ======================================================
	// Get pointer access to host object
	// ======================================================
	involvedComponents.theHost.hobject->request_specialization((jvxHandle**)&involvedHost.hHost,
		&cpType, &multObj);
	assert(involvedHost.hHost);

	// ======================================================
	// Check component type of specialization
	// ======================================================
	assert(involvedHost.hHost && (cpType.tp == JVX_COMPONENT_HOST));

	// ==============================================================================

	involvedComponents.theHost.hFHost = static_cast<IjvxFactoryHost*>(involvedHost.hHost);
	return res;
}

jvxErrorType
JVX_APPHOST_CLASSNAME::closeHostFactory(jvxApiString* errorMessage, jvxHandle* context)
{
#ifdef JVX_HOST_USE_ONLY_STATIC_OBJECTS
	jvxHAppHost_nd_terminate(this->involvedComponents.theHost.hobject);
#else
	jvxHAppHost_terminate(this->involvedComponents.theHost.hobject);
#endif
	involvedHost.hHost = NULL;
	return JVX_NO_ERROR;
}

jvxErrorType
JVX_APPHOST_CLASSNAME::shutdownHostFactory(jvxApiString* errorMessage, jvxHandle* context)
{
	jvxSize i;
	// Here, we deactivate all dynamic objects found in slots. We need to do this to
	// allow text log in deactivate callbacks
	this->involvedHost.hHost->system_about_to_shutdown();

	// HINT HK 21.04.2023: Changed this part: it browsed through all components previously but
	// it is better to handle the automation components after all the others!!!
	
	for (i = JVX_COMPONENT_UNKNOWN + 1; i < JVX_MAIN_COMPONENT_LIMIT; i++)
	{
		jvxSize j, k;
		jvxSize szSlots = 0;
		jvxSize szSubSlots = 0;
		jvxComponentType tpParent = JVX_COMPONENT_UNKNOWN;
		jvxState stat = JVX_STATE_NONE;
		jvxComponentIdentification tpLoc((jvxComponentType)i, 0, 0);

		this->involvedHost.hHost->number_slots_component_system(tpLoc, &szSlots, NULL, nullptr, nullptr);
		for (j = 0; j < szSlots; j++)
		{
			jvxSize szSubSlots = 0;
			tpLoc.slotid = j;
			this->involvedHost.hHost->number_slots_component_system(tpLoc, NULL, &szSubSlots, nullptr, nullptr);
			for (k = 0; k < szSubSlots; k++)
			{
				tpLoc.slotsubid = k;
				this->involvedHost.hHost->state_selected_component(tpLoc, &stat);
				if (stat == JVX_STATE_ACTIVE)
				{
					this->involvedHost.hHost->deactivate_selected_component(tpLoc);
					//subWidgets.main.theWidget->inform_inactive(tpLoc);
				}

				this->involvedHost.hHost->state_selected_component(tpLoc, &stat);
				if (stat == JVX_STATE_SELECTED)
				{
					this->involvedHost.hHost->unselect_selected_component(tpLoc);
				}
			}
		}
	}

	// HINT HK 21.04.2023: Moved this part here from 
	// member function on_components_stop AA
	// Here we disconnect automation
	if (confHostFeatures)
	{
		if (confHostFeatures->automation.if_autoconnect)
		{
			involvedComponents.theHost.hFHost->remove_external_interface(confHostFeatures->automation.if_autoconnect, JVX_INTERFACE_AUTO_DATA_CONNECT);
		}

		// If we have received the pointers from the component, release it.
		if (if_report_automate_ || if_autoconnect_)
		{
			IjvxToolsHost* tools = nullptr;
			involvedHost.hHost->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&tools));
			if (tools)
			{
				IjvxSimpleComponent* automation = nullptr;
				jvxComponentIdentification tp = JVX_COMPONENT_SYSTEM_AUTOMATION;
				tp.slotid = JVX_SIZE_UNSELECTED;
				refComp< IjvxSimpleComponent> elmRet = reqRefTool<IjvxSimpleComponent>(tools, tp, JVX_SIZE_UNSELECTED, confHostFeatures->automation.mod_selection);
				automation = elmRet.cpPtr;
				involvedHost.hHost->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, tools);
				if (automation)
				{
					IjvxReportSystem* rep = nullptr;
					IjvxAutoDataConnect* dcon = nullptr;
					if (if_report_automate_)
					{
						automation->return_hidden_interface(JVX_INTERFACE_REPORT_SYSTEM, reinterpret_cast<jvxHandle*>(if_report_automate_));
						if_report_automate_ = nullptr;
						confHostFeatures->automation.if_report_automate = nullptr;
						reqHandle.automationReport = nullptr;
					}
					if (if_autoconnect_)
					{
						automation->return_hidden_interface(JVX_INTERFACE_AUTO_DATA_CONNECT, reinterpret_cast<jvxHandle*>(if_autoconnect_));
						if_autoconnect_ = nullptr;
						confHostFeatures->automation.if_autoconnect = nullptr;
					}
				}
			}
		}
		// else: if we got the two automation pointers from the confHostFeatures struct, we should not touch them
		deactivate_default_components_host(confHostFeatures->lst_ModulesOnStart, involvedHost.hHost, true);
	}

	// Here the system components as last
	for (i = JVX_MAIN_COMPONENT_LIMIT; i < JVX_COMPONENT_ALL_LIMIT ; i++)
	{
		jvxSize j, k;
		jvxSize szSlots = 0;
		jvxSize szSubSlots = 0;
		jvxComponentType tpParent = JVX_COMPONENT_UNKNOWN;
		jvxState stat = JVX_STATE_NONE;
		jvxComponentIdentification tpLoc((jvxComponentType)i, 0, 0);

		this->involvedHost.hHost->number_slots_component_system(tpLoc, &szSlots, NULL, nullptr, nullptr);
		for (j = 0; j < szSlots; j++)
		{
			jvxSize szSubSlots = 0;
			tpLoc.slotid = j;
			this->involvedHost.hHost->number_slots_component_system(tpLoc, NULL, &szSubSlots, nullptr, nullptr);
			for (k = 0; k < szSubSlots; k++)
			{
				tpLoc.slotsubid = k;
				this->involvedHost.hHost->state_selected_component(tpLoc, &stat);
				if (stat == JVX_STATE_ACTIVE)
				{
					this->involvedHost.hHost->deactivate_selected_component(tpLoc);
					//subWidgets.main.theWidget->inform_inactive(tpLoc);
				}

				this->involvedHost.hHost->state_selected_component(tpLoc, &stat);
				if (stat == JVX_STATE_SELECTED)
				{
					this->involvedHost.hHost->unselect_selected_component(tpLoc);
				}
			}
		}
	}
	// HINT HK STOP
	if (confHostFeatures)
	{
		// We have to reset the host features since there is no real term function 
		confHostFeatures->automation.if_report_automate = nullptr;
		confHostFeatures->automation.if_autoconnect = nullptr;
		confHostFeatures->automation.mod_selection = nullptr;
	}
	return JVX_NO_ERROR;
}

void 
JVX_APPHOST_CLASSNAME::init_command_line_parameters(IjvxCommandLine* commLine)
{
		 		_command_line_parameters_hosttype.num_slots_max = 1;
			_command_line_parameters_hosttype.num_subslots_max = 1;

}

void
JVX_APPHOST_CLASSNAME::register_command_line_parameters(IjvxCommandLine* commLine)
{
	commLine->register_option("--num_slots_max", "-smax", "Specify default number of slots", "1", true, JVX_DATAFORMAT_SIZE);
	commLine->register_option("--num_subslots_max", "-ssmax", "Specify default number of subslots", "1", true, JVX_DATAFORMAT_SIZE);

}

void
JVX_APPHOST_CLASSNAME::read_command_line_parameters(IjvxCommandLine* commLine)
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSize opt_sz = 0;

	opt_sz = 0;
	resL = commLine->content_entry_option("--num_slots_max", 0, &opt_sz, JVX_DATAFORMAT_SIZE);
	if (resL == JVX_NO_ERROR)
	{
		_command_line_parameters_hosttype.num_slots_max = opt_sz;
	}

	opt_sz = 0;
	resL = commLine->content_entry_option("--num_subslots_max", 0, &opt_sz, JVX_DATAFORMAT_SIZE);
	if (resL == JVX_NO_ERROR)
	{
		_command_line_parameters_hosttype.num_subslots_max = opt_sz;
	}

}


jvxErrorType
JVX_APPHOST_CLASSNAME::boot_initialize_base(jvxSize* numSlots)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string txtOut;	

	//=================================================================
	// Add the types to be handled by host
	//=================================================================

	IjvxHostTypeHandler* theTypeHandler = NULL;
	res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_HOSTTYPEHANDLER, 
		reinterpret_cast<jvxHandle**>(&theTypeHandler));
	if ((res == JVX_NO_ERROR) && theTypeHandler)
	{
		// Verify the integrity of the system: the component list must be properly filled.
		// If we crash here, the definition in TjvxTypes.cpp, line 5, is broken since we 
		// added or removed a component type. Please check that position
		if (
			(theClassAssociation[JVX_COMPONENT_ALL_LIMIT].comp_class != jvxComponentTypeClass::JVX_COMPONENT_TYPE_LIMIT) ||
			(theClassAssociation[JVX_COMPONENT_ALL_LIMIT].comp_sec_type != JVX_COMPONENT_ALL_LIMIT))
		{
			std::cout << __FUNCTION__ << ": Fatal error: the component settings are broken due to a malfunctioning system setup at compile time." << std::endl;
			assert(0);
			exit(0);
		}
		// ========================================================================================================

		for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
		{
			jvxComponentType tp[2];

			switch (theClassAssociation[i].comp_class)
			{
			case jvxComponentTypeClass::JVX_COMPONENT_TYPE_TECHNOLOGY:
				tp[0] = (jvxComponentType)i;
				tp[1] = (jvxComponentType)theClassAssociation[i].comp_sec_type;

				if (
					(numSlots[i] > 0) &&
					(numSlots[theClassAssociation[i].comp_sec_type] > 0))
				{
					txtOut = "Entry [" + jvx_size2String(i) + ", " + (std::string)jvxComponentType_txt(i) + "]: Setting up ";
					txtOut += jvx_size2String(numSlots[i]);
					txtOut += " slots and ";
					txtOut += jvx_size2String(numSlots[theClassAssociation[i].comp_sec_type]);
					txtOut += " subslots for component types ";
					txtOut += jvxComponentType_txt(tp[0]);
					txtOut += " and ";
					txtOut += jvxComponentType_txt(tp[1]);
					txtOut += ".";
					if (_command_line_parameters.verbose_slots)
					{
						if ((report) && (!_command_line_parameters.out_cout))
						{
							report->report_simple_text_message(txtOut.c_str(), JVX_REPORT_PRIORITY_VERBOSE);
						}
						else
						{
							std::cout << txtOut << std::endl;
						}
					}
					res = theTypeHandler->add_type_host(tp, 2, theClassAssociation[i].description, theClassAssociation[i].config_token,
						theClassAssociation[i].comp_class,
						numSlots[i],
						numSlots[theClassAssociation[i].comp_sec_type], 
						theClassAssociation[i].comp_child_class);
					assert(res == JVX_NO_ERROR);
				}
				break;
			case jvxComponentTypeClass::JVX_COMPONENT_TYPE_NODE:
				tp[0] = (jvxComponentType)i;
				tp[1] = JVX_COMPONENT_UNKNOWN;

				if (numSlots[i] > 0)
				{
					if (_command_line_parameters.verbose_slots)
					{
						txtOut = "Entry [" + jvx_size2String(i) + ", " + (std::string)jvxComponentType_txt(i) + "]: Setting up ";
						txtOut += jvx_size2String(numSlots[i]);
						txtOut += " slots for component type ";
						txtOut += jvxComponentType_txt(tp[0]);
						txtOut += ".";
						if ((report) && (!_command_line_parameters.out_cout))
						{
							report->report_simple_text_message(txtOut.c_str(), JVX_REPORT_PRIORITY_VERBOSE);
						}
						else
						{
							std::cout << txtOut << std::endl;
						}
					}
					res = theTypeHandler->add_type_host(tp, 1, theClassAssociation[i].description, theClassAssociation[i].config_token,
						theClassAssociation[i].comp_class, numSlots[i], 0, theClassAssociation[i].comp_child_class);
					assert(res == JVX_NO_ERROR);
				}
				break;

			case jvxComponentTypeClass::JVX_COMPONENT_TYPE_SIMPLE:
				tp[0] = (jvxComponentType)i;
				tp[1] = JVX_COMPONENT_UNKNOWN;

				if (numSlots[i] > 0)
				{
					if (_command_line_parameters.verbose_slots)
					{
						txtOut = "Entry [" + jvx_size2String(i) + ", " + (std::string)jvxComponentType_txt(i) + "]: Setting up ";
						txtOut += jvx_size2String(numSlots[i]);
						txtOut += " slots for component type ";
						txtOut += jvxComponentType_txt(tp[0]);
						txtOut += ".";
						if ((report) && (!_command_line_parameters.out_cout))
						{
							report->report_simple_text_message(txtOut.c_str(), JVX_REPORT_PRIORITY_VERBOSE);
						}
						else
						{
							std::cout << txtOut << std::endl;
						}
					}
					res = theTypeHandler->add_type_host(tp, 1, theClassAssociation[i].description, theClassAssociation[i].config_token,
						theClassAssociation[i].comp_class,
						numSlots[i], 0, theClassAssociation[i].comp_child_class);
					assert(res == JVX_NO_ERROR);
				}
				break;
			}
		}
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_HOSTTYPEHANDLER, reinterpret_cast<jvxHandle*>(theTypeHandler));
	}

	reqHandle.hostRef = involvedHost.hHost;

	return boot_initialize_product();
}

jvxErrorType
JVX_APPHOST_CLASSNAME::reference_properties(const jvxComponentIdentification& tpId, IjvxAccessProperties** accProps)
{
	std::map<jvxComponentIdentification, jvx_componentIdPropsCombo>::iterator elm = theRegisteredComponentReferenceTriples.find(tpId);
	if (elm != theRegisteredComponentReferenceTriples.end())
	{
		if (accProps)
		{
			*accProps = elm->second.accProps;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

jvxErrorType
JVX_APPHOST_CLASSNAME::shutdown_terminate_base()
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string txtOut;	

	shutdown_terminate_product();

	reqHandle.hostRef = nullptr;

	// The following two actions only work on host app types, factory host does not support it
	IjvxHostTypeHandler* theTypeHandler = NULL;
	res = this->involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_HOSTTYPEHANDLER, reinterpret_cast<jvxHandle**>(&theTypeHandler));
	if ((res == JVX_NO_ERROR) && theTypeHandler)
	{
		res = theTypeHandler->remove_all_host_types();
		assert(res == JVX_NO_ERROR);
		this->involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_HOSTTYPEHANDLER, reinterpret_cast<jvxHandle*>(theTypeHandler));
	}
	
	return JVX_NO_ERROR;
}

// ==========================================================================

jvxErrorType
JVX_APPHOST_CLASSNAME::on_connection_rules_started()
{
	IjvxDataConnections* theConnections = NULL;
	involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theConnections));
	if (theConnections)
	{
		jvx_default_connection_rules_add(theConnections);
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(theConnections));
	}
	onConnectionRulesStarted = true;

	return JVX_NO_ERROR;
}

jvxErrorType
JVX_APPHOST_CLASSNAME::on_components_before_configure()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (confHostFeatures)
	{
		// Here we have the chance to load the automation references from a compnent of type JVX_COMPONENT_SYSTEM_AUTOMATE.
		// The interfaces must then be accepted.	
		// HK 19.01.2022: Changed the order: need automation already before starting to load the components
		if (confHostFeatures->automation.mod_selection)
		{
			IjvxToolsHost* tools = nullptr;
			involvedHost.hHost->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&tools));
			if (tools)
			{
				IjvxSimpleComponent* automation = nullptr;
				jvxComponentIdentification tp = JVX_COMPONENT_SYSTEM_AUTOMATION;
				tp.slotid = JVX_SIZE_UNSELECTED;
				refComp< IjvxSimpleComponent> elmRet = reqRefTool<IjvxSimpleComponent>(tools, tp, JVX_SIZE_UNSELECTED, confHostFeatures->automation.mod_selection);
				automation = elmRet.cpPtr;
				involvedHost.hHost->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, tools);
				if (automation)
				{
					IjvxReportSystem* rep = nullptr;
					IjvxAutoDataConnect* dcon = nullptr;

					// If the external pointer is unset we use the one from the automation component
					if (!confHostFeatures->automation.if_report_automate)
					{
						automation->request_hidden_interface(JVX_INTERFACE_REPORT_SYSTEM, reinterpret_cast<jvxHandle**>(&if_report_automate_));
						confHostFeatures->automation.if_report_automate = if_report_automate_;
						reqHandle.automationReport = confHostFeatures->automation.if_report_automate;
					}

					// If the external pointer is unset we use the one from the automation component
					if (!confHostFeatures->automation.if_autoconnect)
					{
						automation->request_hidden_interface(JVX_INTERFACE_AUTO_DATA_CONNECT, reinterpret_cast<jvxHandle**>(&if_autoconnect_));
						confHostFeatures->automation.if_autoconnect = if_autoconnect_;
					}
				}
				else
				{
					std::cout << __FUNCTION__ << ": Failed to open Automation module <" << confHostFeatures->automation.mod_selection << ">." << std::endl;
				}
			}
		}
		
		// Here we accept the autoconnect interface. This interface always runs as part of the main process.
		// Therefore it can be installed here without interfering with the UI.
		// The IjvxReport interface confHostFeatures->automation.if_report_automate, however, can only be installed in the 
		// UI system or in a component which can be triggered from within the UI thread
		if (confHostFeatures->automation.if_autoconnect)
		{
			involvedComponents.theHost.hFHost->add_external_interface(confHostFeatures->automation.if_autoconnect, JVX_INTERFACE_AUTO_DATA_CONNECT);			
		}

		// Here, we start all system components. System components are loaded before the
		// other components to form kind of over-the-others system, e.g. to automate functions
		// HK 19.01.2022: Changed the order: need automation already before starting to load the components
		activate_default_components_host(confHostFeatures->lst_ModulesOnStart, involvedHost.hHost, true, confHostFeatures->verbose_ModulesOnStart);

	}
	onComponentsBeforeConfig = true;

	return JVX_NO_ERROR;
}

jvxErrorType
JVX_APPHOST_CLASSNAME::on_components_configured()
{
	//=================================================================
	// Activate default components
	//=================================================================
	if (confHostFeatures)
	{
		// Activate all default components if required
		activate_default_components_host(confHostFeatures->lst_ModulesOnStart, involvedHost.hHost, false, confHostFeatures->verbose_ModulesOnStart);

		jvxErrorType res = involvedComponents.theHost.hFHost->system_ready();
		if (res != JVX_NO_ERROR)
		{
			std::cout << "System was started but host reported to be not ready, error reason: <" << jvxErrorType_descr(res) << ">." << std::endl;
		}
	}
	onComponentsConfigured = true;

	return JVX_NO_ERROR;
}

jvxErrorType
JVX_APPHOST_CLASSNAME::on_sequencer_started()
{
	IjvxSequencer* theSequencer = NULL;
	// Call the weakly defined callback to add sequencer steps
	involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSequencer));
	if (theSequencer)
	{
		jvx_default_sequence_add(theSequencer);
		theSequencer->update_config_sequences();
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle*>(theSequencer));
	}
	onSequencerStarted = true;

	return JVX_NO_ERROR;
}

jvxErrorType
JVX_APPHOST_CLASSNAME::on_connections_started()
{
	IjvxDataConnections* theConnections = NULL;
	involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theConnections));
	if (theConnections)
	{
		jvx_default_connections_add(theConnections);
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(theConnections));
	}
	onConnectionsStarted = true;
	return JVX_NO_ERROR;
}

// ========================================================================================

void
JVX_APPHOST_CLASSNAME::boot_prepare_host_start()
{
	onConnectionRulesStarted = false;
	onComponentsBeforeConfig = false;
	onComponentsConfigured = false;
	onSequencerStarted = false;
	onConnectionsStarted = false;
}

/**
 * This function is called once the boot propcess to bring the host into a state where it may be started is complete.
 * We can expect all functions to run in a single thread until this moment and we will stop to allow some of the functions
 * at a time later than right now, e.g. to add an external interface.
 * In this function, all steps which are run in the put_configuration function, can be triggered in case there just was no 
 * configuration present. It is like the cleanup all in the boot process
 */
void
JVX_APPHOST_CLASSNAME::boot_prepare_host_stop()
{
	if (!onConnectionRulesStarted)
	{
		this->on_connection_rules_started();
	}

	if (!onComponentsBeforeConfig)
	{
		this->on_components_before_configure();
	}

	if (!onComponentsConfigured)
	{
		this->on_components_configured();
	}

	if (!onSequencerStarted)
	{
		this->on_sequencer_started();
	}

	if (!onConnectionsStarted)
	{
		this->on_connections_started();
	}

	involvedHost.hHost->report_boot_complete(true);
}

jvxErrorType 
JVX_APPHOST_CLASSNAME::on_sequencer_stop()
{
	IjvxSequencer* theSequencer = NULL;
	onSequencerStarted = false;
	jvxErrorType res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_SEQUENCER, reinterpret_cast<jvxHandle**>(&theSequencer));
	if (theSequencer)
	{
		// Remove all sequences and clear!
		jvxErrorType resS = JVX_NO_ERROR;
		while (resS == JVX_NO_ERROR)
		{
			resS = theSequencer->remove_sequence(0);
		}
	}
	return res;
}

jvxErrorType 
JVX_APPHOST_CLASSNAME::on_connections_stop()
{
	IjvxDataConnections* theConnections = NULL;
	onConnectionsStarted = false;
	jvxErrorType res = this->involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theConnections));
	if ((res == JVX_NO_ERROR) && theConnections)
	{
		res = theConnections->remove_all_connection();
		assert(res == JVX_NO_ERROR);
		this->involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(theConnections));
	}
	return res;
}

jvxErrorType 
JVX_APPHOST_CLASSNAME::on_components_stop()
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxToolsHost* tools = nullptr;

	// First part
	onComponentsConfigured = false;
	
	// Tell all components that we are about to stop
	res = involvedComponents.theHost.hFHost->system_about_to_shutdown();

	if (res != JVX_NO_ERROR)
	{
		std::cout << "System was started but host reported to be not ready, error reason: <" << jvxErrorType_descr(res) << ">." << std::endl;
	}

	if (confHostFeatures)
	{
		deactivate_default_components_host(confHostFeatures->lst_ModulesOnStart, involvedHost.hHost, false);
	}

	// ====================================================================================================

	// Second part
	onComponentsBeforeConfig = false;	

	// HK HINT: AA

	return res;
}

jvxErrorType 
JVX_APPHOST_CLASSNAME::on_connectionrules_stop()
{
	IjvxDataConnections* theConnections = NULL;
	onConnectionRulesStarted = false;

	jvxErrorType res = this->involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle**>(&theConnections));
	if ((res == JVX_NO_ERROR) && theConnections)
	{
		res = theConnections->remove_all_connection_rules();
		assert(res == JVX_NO_ERROR);
		this->involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_DATA_CONNECTIONS, reinterpret_cast<jvxHandle*>(theConnections));
	}
	return res;
}

// =============================================================================
void
JVX_APPHOST_CLASSNAME::shutdown_postprocess_host()
{
	jvxErrorType res = JVX_NO_ERROR;

	// Indicate that we are about to shutdown
	involvedHost.hHost->report_boot_complete(false);

	on_connections_stop();
	
	on_sequencer_stop();

	on_components_stop();

	on_connectionrules_stop();
}