#include "jvxLibHost.h"
#include "allHostsStatic_common.h"

#include "jvxTconfigProcessor.h"
#include "jvxTSystemTextLog.h"
#include "jvxTThreads.h"
#include "jvxTDataLogger.h"

#include "jvxTDataConverter.h"
#include "jvxTResampler.h"
#include "jvxTrtAudioFileReader.h"
#include "jvxTrtAudioFileWriter.h"
#include "jvxAuTGenericWrapper.h"

extern "C"
{
	// These symbols may be weakly linked (optional) or required (mandatory)
#ifdef LINK_COMPONENTS_WEAK_SYMBOL

#define FUNC_CORE_PROT_DECLARE jvx_configure_factoryhost_features
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_configure_host_features_local
#define FUNC_CORE_PROT_ARGS configureHost_features* theFeaturesA
#define FUNC_CORE_PROT_RETURNVAL jvxErrorType

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_configure_factoryhost_features=jvx_configure_factoryhost_features_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_configure_factoryhost_features=_jvx_configure_factoryhost_features_local")
#endif

#include "platform/jvx_platform_weak_defines.h"
	{
		std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
		// Default implementation does just nothing
		return(JVX_NO_ERROR);
	}

#undef FUNC_CORE_PROT_DECLARE
#undef FUNC_CORE_PROT_DECLARE_LOCAL
#undef FUNC_CORE_PROT_ARGS
#undef FUNC_CORE_PROT_RETURNVAL


#define FUNC_CORE_PROT_DECLARE jvx_invalidate_factoryhost_features
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_invalidate_factoryhost_features_local
#define FUNC_CORE_PROT_ARGS configureHost_features* theFeaturesA
#define FUNC_CORE_PROT_RETURNVAL jvxErrorType

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_invalidate_factoryhost_features=jvx_invalidate_factoryhost_features_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_invalidate_factoryhost_features=_jvx_invalidate_factoryhost_features_local")
#endif

#include "platform/jvx_platform_weak_defines.h"
	{
		std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
		// Default implementation does just nothing
		return(JVX_NO_ERROR);
	}

#undef FUNC_CORE_PROT_DECLARE
#undef FUNC_CORE_PROT_DECLARE_LOCAL
#undef FUNC_CORE_PROT_ARGS
#undef FUNC_CORE_PROT_RETURNVAL


#define FUNC_CORE_PROT_DECLARE jvx_access_link_objects
#define FUNC_CORE_PROT_DECLARE_LOCAL jvx_access_link_objects_local
#define FUNC_CORE_PROT_ARGS jvxInitObject_tp* funcInit, jvxTerminateObject_tp* funcTerm, jvxApiString* description, jvxComponentType tp, jvxSize id, jvxCBitField* stateMachineHintFlags
#define FUNC_CORE_PROT_RETURNVAL jvxErrorType

#if defined(JVX_SYS_WINDOWS_MSVC_64BIT)
#pragma comment(linker, "/alternatename:jvx_access_link_objects=jvx_access_link_objects_local")
#elif defined(JVX_SYS_WINDOWS_MSVC_32BIT)
#pragma comment(linker, "/alternatename:_jvx_access_link_objects=_jvx_access_link_objects_local")
#endif

#include "platform/jvx_platform_weak_defines.h"
	{
		std::cout << __FUNCTION__ << ": Default (weak) implementation chosen." << std::endl;
		// Default implementation does just nothing
		return(JVX_ERROR_ELEMENT_NOT_FOUND);
	}

#else

	extern jvxErrorType jvx_configure_factoryhost_features(
		configureFactoryHost_features* theFeaturesA);
		
	extern jvxErrorType jvx_access_link_objects(
		jvxInitObject_tp* funcInit, 
		jvxTerminateObject_tp* funcTerm, 
		jvxApiString* description, 
		jvxComponentType tp, jvxSize id);
#endif
}


// ======================================================================
// JVX_APP_FACTORY_HOST_CLASSNAME
// ======================================================================

jvxErrorType
jvxLibHost::boot_initialize_specific(jvxApiString* errloc)
{
	jvxErrorType res = JVX_NO_ERROR;
	// ==========================================================================================
	IjvxProperties* theProps = NULL;
	jvxSize i, cnt;
	// char descriptionComponent[JVX_MAXSTRING];
	jvxCallManagerProperties callGate;
	oneAddedStaticComponent comp;

	/**
	 * TODO: We may add an event loop at this point
	 * involvedComponents.theHost.hFHost->add_external_interface(reinterpret_cast<jvxHandle*>(static_cast<IjvxEventLoop*>(xy)), JVX_INTERFACE_EVENTLOOP);
	 */
	 
	IjvxPropertyAttach* thePropExp = nullptr;
	genLibHost::init_all();
	genLibHost::allocate_all();
	genLibHost::register_all(static_cast<CjvxProperties*>(this));
	genLibHost::register_callbacks(static_cast<CjvxProperties*>(this), cb_set_options, this, nullptr);

	_common_set_properties.propIdSpan = 20;
	_common_set_min.theState = JVX_STATE_ACTIVE;
	
	/*
	 * ===============================================================================================================
	 * STEP I: Install some additional properties if desired. These can be used to control host via property callbacks
	 * In this approach we attach the local configuration to the host.
	 * ===============================================================================================================
	 */

	jvxErrorType resL = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_PROPERTY_ATTACH, (jvxHandle**)&thePropExp);
	if (thePropExp)
	{
		thePropExp->attach_property_submodule("jvxLibHost", static_cast<IjvxProperties*>(this));
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_PROPERTY_ATTACH, (jvxHandle*)thePropExp);
		thePropExp = nullptr;
	}

	IjvxConfigurationAttach* theCfgAtt = nullptr;
	resL = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_ATTACH, (jvxHandle**)&theCfgAtt);
	if (theCfgAtt)
	{
		theCfgAtt->attach_configuration_submodule("jvxLibHost", static_cast<IjvxConfiguration*>(this));
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION_ATTACH, (jvxHandle*)theCfgAtt);
		theCfgAtt = nullptr;
	}

	/*
	res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_PROPERTIES, (jvxHandle**)&theProps);
	if (theProps)
	{
		jPAD address("/ext_callbacks/cb_save_config");
		theCallback_saveconfig.cb = cb_save_config;
		theCallback_saveconfig.cb_priv = reinterpret_cast<jvxHandle*>(this);
		jvxErrorType resL = theProps->install_property_reference(callGate, jvx::propertyRawPointerType::CjvxRawPointerTypeCallback(&theCallback_saveconfig),address);

		address.reset("/ext_callbacks/cb_xchg_property");
		theCallback_exchg_property.cb = cb_xchg_property;
		theCallback_exchg_property.cb_priv = reinterpret_cast<jvxHandle*>(this);
		//jvx_set_property(theProps, &theCallback_exchg_property, 0, 1, JVX_DATAFORMAT_CALLBACK, true, "/ext_callbacks/cb_xchg_property", callGate);
		resL = theProps->install_property_reference(callGate, jvx::propertyRawPointerType::CjvxRawPointerTypeCallback(&theCallback_exchg_property), address);

		address.reset("/ext_callbacks/cb_shutdown");
		theCallback_shutdown.cb = cb_shutdown;
		theCallback_shutdown.cb_priv = reinterpret_cast<jvxHandle*>(this);
		//jvx_set_property(theProps, &theCallback_shutdown, 0, 1, JVX_DATAFORMAT_CALLBACK, true, "/ext_callbacks/cb_shutdown", callGate);
		resL = theProps->install_property_reference(callGate, jvx::propertyRawPointerType::CjvxRawPointerTypeCallback(&theCallback_shutdown), address);

		address.reset("/ext_callbacks/cb_restart");
		theCallback_restart.cb = cb_restart;
		theCallback_restart.cb_priv = reinterpret_cast<jvxHandle*>(this);
		//jvx_set_property(theProps, &theCallback_restart, 0, 1, JVX_DATAFORMAT_CALLBACK, true, "/ext_callbacks/cb_restart", callGate);
		resL = theProps->install_property_reference(callGate, jvx::propertyRawPointerType::CjvxRawPointerTypeCallback(&theCallback_restart), address);

		/*
		supports_shutdown = false;
		linkedPriFrontend.fe->request_property(JVX_FRONTEND_SUPPORTS_SHUTDOWN, &supports_shutdown);
		if (supports_shutdown)
		{
			theCallback_shutdown.cb = cb_shutdown;
			theCallback_shutdown.cb_priv = reinterpret_cast<jvxHandle*>(this);
			jvx_set_property(theProps, &theCallback_shutdown, 0, 1, JVX_DATAFORMAT_CALLBACK, true, "/ext_callbacks/cb_shutdown", callGate);

			theCallback_restart.cb = cb_restart;
			theCallback_restart.cb_priv = reinterpret_cast<jvxHandle*>(this);
			jvx_set_property(theProps, &theCallback_restart, 0, 1, JVX_DATAFORMAT_CALLBACK, true, "/ext_callbacks/cb_restart", callGate);
		}
		* ** /
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_PROPERTIES, theProps);
	}
	theProps = NULL;
	*/
	
	/*
	 *============================================================================================================== =
	 *STEP II : Pre-install default MAX number of components to be used
	 * ============================================================================================================== 
	 */
	JVX_START_SLOTS_BASE(theHostFeatures.numSlotsComponents,
		_command_line_parameters_hosttype.num_slots_max,
		_command_line_parameters_hosttype.num_subslots_max);
	JVX_START_SLOTS_SUBPRODUCT(theHostFeatures.numSlotsComponents,
		_command_line_parameters_hosttype.num_slots_max,
		_command_line_parameters_hosttype.num_subslots_max);

	/*
	 *============================================================================================================== =
     *STEP III : Install report handle to allow backward reports
     * ==============================================================================================================
     */
	// Pass report handle to the UI elements
	theHostFeatures.if_report = static_cast<IjvxReport*>(this);

	// Pass host handle to the UI elements
	theHostFeatures.fHost = involvedComponents.theHost.hFHost;
	theHostFeatures.hHost = involvedHost.hHost;

	/*
	 *============================================================================================================== =
	 *STEP IV : Request host type feature options from the application config callback
	 * ==============================================================================================================
	 */
	jvx_configure_factoryhost_features(static_cast<configureFactoryHost_features*>(&theHostFeatures));

	// Here, start the "real" initialization
	boot_initialize_base(theHostFeatures.numSlotsComponents);

	/*
	 *============================================================================================================== =
	 *STEP V : Attach all system specific common components
	 * ==============================================================================================================
	 */

	 //=================================================================
	 // Open all static libraries and add to host
	 // Do not add before since the accepted types must have been specified
	 //=================================================================
	LOAD_ONE_MODULE_LIB_FULL(jvxTSystemTextLog_init,
		jvxTSystemTextLog_terminate, "Text Log",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTconfigProcessor_init,
		jvxTconfigProcessor_terminate, "Config Parser",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTThreads_init,
		jvxTThreads_terminate, "Threads",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTDataLogger_init,
		jvxTDataLogger_terminate, "Data Logger",
		involvedComponents.addedStaticObjects,
		involvedComponents.theHost.hFHost);
	
	// System components before, subproduct (audio) components next
	LOAD_ONE_MODULE_LIB_FULL(jvxTDataConverter_init, 
		jvxTDataConverter_terminate, "Data Converter", 
		involvedComponents.addedStaticObjects, 
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTResampler_init, 
		jvxTResampler_terminate, "Resampler", 
		involvedComponents.addedStaticObjects, 
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTrtAudioFileReader_init, 
		jvxTrtAudioFileReader_terminate, "RT Audio Reader", 
		involvedComponents.addedStaticObjects, 
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxTrtAudioFileWriter_init, 
		jvxTrtAudioFileWriter_terminate, "RT Audio Writer", 
		involvedComponents.addedStaticObjects, 
		involvedComponents.theHost.hFHost);
	LOAD_ONE_MODULE_LIB_FULL(jvxAuTGenericWrapper_init, 
		jvxAuTGenericWrapper_terminate, "Generic Wrapper Audio", 
		involvedComponents.addedStaticObjects, 
		involvedComponents.theHost.hFHost);

	// Only host to blacklist
	involvedComponents.theHost.hFHost->add_component_load_blacklist(JVX_COMPONENT_HOST);

	/*
	 *============================================================================================================== =
	 *STEP VI : Request the application specific component library in this callback
	 * ==============================================================================================================
	 */
	for (i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
	{
		cnt = 0;
		while (1)
		{
			jvxApiString descriptionComponent;
			comp.reset();
			jvxErrorType resL = jvx_access_link_objects(&comp.funcInit, &comp.funcTerm, &descriptionComponent, (jvxComponentType)i, cnt);
			if (resL == JVX_NO_ERROR)
			{
				comp.theStaticObject = NULL;
				resL = comp.funcInit(&comp.theStaticObject, &comp.theStaticGlobal, NULL);
				assert(resL == JVX_NO_ERROR);
				assert(comp.theStaticObject);
				resL = involvedComponents.theHost.hFHost->add_external_component(comp.theStaticObject, comp.theStaticGlobal,
					descriptionComponent.c_str(), true, comp.funcInit, comp.funcTerm);
				if (resL == JVX_NO_ERROR)
				{
					involvedComponents.addedStaticObjects.push_back(comp);
				}
				else
				{
					comp.funcTerm(comp.theStaticObject);
				}
			}
			else
			{
				break;
			}
			cnt++;
		}
	}

	/*
	 *============================================================================================================== =
	 *STEP VII : Attach component load filter. The callback for the load filter is part of the host_config struct
	 * ==============================================================================================================
	 */
	involvedComponents.theHost.hFHost->set_component_load_filter_function(
		theHostFeatures.cb_loadfilter, 
		theHostFeatures.cb_loadfilter_priv);
	
	/*
	 *============================================================================================================== =
	 *STEP VIII : Add the interpreter interface
	 * ==============================================================================================================
	 */
#ifdef JVX_LIBHOST_WITH_JSON_SHOW
	involvedComponents.theHost.hFHost->add_external_interface(reinterpret_cast<jvxHandle*>(static_cast<IjvxCommandInterpreter*>(this)), JVX_INTERFACE_COMMAND_INTERPRETER);
#endif

	return JVX_NO_ERROR;
}

jvxErrorType
jvxLibHost::boot_specify_slots_specific()
{
	return JVX_NO_ERROR;
}

jvxErrorType
jvxLibHost::boot_prepare_specific(jvxApiString* errloc)
{
	/*
	 * At this point, we activate the generic wrapper technology as all audio io will be passed through
	 * the respective helper functinalities to allow resampling rebuffering etc.
	 */
	jvxState stat = JVX_STATE_NONE;
	jvxErrorType res = JVX_NO_ERROR;
	IjvxObject* theObject = NULL;
	jvxApiString  fldStr;
	jvxSize num = 0;
	jvxSize foundComp = JVX_SIZE_UNSELECTED;
	jvxSize i;
	jvxBool requiresNewStart = true;
	jvxSize slotId = 0;
	jvxSize slotSubId = 0;
	//jvxComponentIdentification& tpIdT = tpAll[JVX_COMPONENT_AUDIO_TECHNOLOGY];

	// Check technology 
	jvxComponentIdentification tpIdT;
	tpIdT.tp = JVX_COMPONENT_AUDIO_TECHNOLOGY;
	tpIdT.slotid = 0;
	tpIdT.slotsubid = 0;

	jvx_componentIdPropsCombo newRefElm;
	involvedHost.hHost->state_selected_component(tpIdT, &stat);
	if (stat >= JVX_STATE_SELECTED)
	{
		theObject = NULL;
		res = involvedHost.hHost->request_object_selected_component(tpIdT, &theObject);
		assert(res == JVX_NO_ERROR);
		assert(theObject != NULL);

		theObject->descriptor(&fldStr);
		if ((std::string)fldStr.std_str() != "JVX_GENERIC_WRAPPER")
		{

			res = involvedHost.hHost->deactivate_selected_component(tpIdT);
			assert(res == JVX_NO_ERROR);

			jvx_componentIdentification_properties_erase(
				involvedHost.hHost,
				theRegisteredComponentReferenceTriples,
				tpIdT);

			res = involvedHost.hHost->unselect_selected_component(tpIdT);
			assert(res == JVX_NO_ERROR);
		}
		else
		{
			requiresNewStart = false;
		}
	}

	if (requiresNewStart)
	{
		res = involvedHost.hHost->number_components_system(tpIdT, &num);
		foundComp = JVX_SIZE_UNSELECTED;

		for (i = 0; i < num; i++)
		{
			involvedHost.hHost->descriptor_component_system(tpIdT, i, &fldStr);
			if (fldStr.std_str() == (std::string)"JVX_GENERIC_WRAPPER")
			{
				foundComp = i;
				break;
			}
		}

		if (JVX_CHECK_SIZE_UNSELECTED(foundComp))
		{
			assert(0);
		}

		involvedHost.hHost->select_component(tpIdT, foundComp);
	}

	return JVX_NO_ERROR;
}

jvxErrorType
jvxLibHost::boot_start_specific(jvxApiString* errloc)
{
	return JVX_NO_ERROR;
}

jvxErrorType
jvxLibHost::boot_activate_specific(jvxApiString* errloc)
{
	return JVX_NO_ERROR;
}


jvxErrorType
jvxLibHost::shutdown_terminate_specific(jvxApiString* errloc)
{
	jvxSize i;

#ifdef JVX_LIBHOST_WITH_JSON_SHOW
	involvedComponents.theHost.hFHost->remove_external_interface(reinterpret_cast<jvxHandle*>(static_cast<IjvxCommandInterpreter*>(this)), JVX_INTERFACE_COMMAND_INTERPRETER);
#endif

	involvedComponents.theHost.hFHost->set_component_load_filter_function(NULL, NULL);

	// Do not allow that host components are loaded via DLL
	involvedComponents.theHost.hFHost->remove_component_load_blacklist(JVX_COMPONENT_HOST);

	// Remove all library objects
	for (i = 0; i < involvedComponents.addedStaticObjects.size(); i++)
	{
		UNLOAD_ONE_MODULE_LIB_FULL(involvedComponents.addedStaticObjects[i], 
			involvedComponents.theHost.hFHost);
	}
	involvedComponents.addedStaticObjects.clear();

	shutdown_terminate_base();
	
	// Uninstall the callbacks
	/*
	IjvxProperties* theProps = NULL;
	jvxCallManagerProperties callGate;
	jvxErrorType res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_PROPERTIES, (jvxHandle**)&theProps);
	if (theProps)
	{
		jPAD address("/ext_callbacks/cb_save_config");
		jvxErrorType resL = theProps->uninstall_property_reference(callGate, jvx::propertyRawPointerType::CjvxRawPointerTypeCallback(&theCallback_saveconfig), address);
		//jvx_set_property(theProps, nullptr, 0, 1, JVX_DATAFORMAT_CALLBACK, true, "/ext_callbacks/cb_save_config", callGate);
		theCallback_saveconfig.cb = nullptr;
		theCallback_saveconfig.cb_priv = nullptr;

		//jvx_set_property(theProps, nullptr, 0, 1, JVX_DATAFORMAT_CALLBACK, true, "/ext_callbacks/cb_xchg_property", callGate);
		address.reset("/ext_callbacks/cb_xchg_property");
		resL = theProps->uninstall_property_reference(callGate, jvx::propertyRawPointerType::CjvxRawPointerTypeCallback(&theCallback_exchg_property), address);
		theCallback_exchg_property.cb = nullptr;
		theCallback_exchg_property.cb_priv = nullptr;

		//jvx_set_property(theProps, nullptr, 0, 1, JVX_DATAFORMAT_CALLBACK, true, "/ext_callbacks/cb_shutdown", callGate);
		address.reset("/ext_callbacks/cb_shutdown");
		resL = theProps->uninstall_property_reference(callGate, jvx::propertyRawPointerType::CjvxRawPointerTypeCallback(&theCallback_shutdown), address);
		theCallback_shutdown.cb = nullptr;
		theCallback_shutdown.cb_priv = nullptr;

		//jvx_set_property(theProps, nullptr, 0, 1, JVX_DATAFORMAT_CALLBACK, true, "/ext_callbacks/cb_restart", callGate);
		address.reset("/ext_callbacks/cb_restart");
		resL = theProps->uninstall_property_reference(callGate, jvx::propertyRawPointerType::CjvxRawPointerTypeCallback(&theCallback_restart), address);
		theCallback_restart.cb = nullptr;
		theCallback_restart.cb_priv = nullptr;

		/*
		supports_shutdown = false;
		linkedPriFrontend.fe->request_property(JVX_FRONTEND_SUPPORTS_SHUTDOWN, &supports_shutdown);
		if (supports_shutdown)
		{
			theCallback_shutdown.cb = cb_shutdown;
			theCallback_shutdown.cb_priv = reinterpret_cast<jvxHandle*>(this);
			jvx_set_property(theProps, &theCallback_shutdown, 0, 1, JVX_DATAFORMAT_CALLBACK, true, "/ext_callbacks/cb_shutdown", callGate);

			theCallback_restart.cb = cb_restart;
			theCallback_restart.cb_priv = reinterpret_cast<jvxHandle*>(this);
			jvx_set_property(theProps, &theCallback_restart, 0, 1, JVX_DATAFORMAT_CALLBACK, true, "/ext_callbacks/cb_restart", callGate);
		}
		* ** /
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_PROPERTIES, theProps);
	}
	theProps = NULL;
	*/

	IjvxConfigurationAttach* theCfgAtt = nullptr;
	jvxErrorType resL = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_ATTACH, (jvxHandle**)&theCfgAtt);
	if (theCfgAtt)
	{
		theCfgAtt->detach_configuration_submodule( static_cast<IjvxConfiguration*>(this));
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION_ATTACH, (jvxHandle*)theCfgAtt);
		theCfgAtt = nullptr;
	}


	IjvxPropertyAttach* thePropExp = nullptr;

	resL = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_PROPERTY_ATTACH, (jvxHandle**)&thePropExp);
	if (thePropExp)
	{
		thePropExp->detach_property_submodule(static_cast<IjvxProperties*>(this));
		involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_PROPERTY_ATTACH, (jvxHandle*)thePropExp);
		thePropExp = nullptr;
	}
	genLibHost::unregister_all(static_cast<CjvxProperties*>(this));
	genLibHost::deallocate_all();
	_common_set_min.theState = JVX_STATE_NONE;
	_common_set_properties.propIdSpan = JVX_SIZE_UNSELECTED;

	return JVX_NO_ERROR;
}

jvxErrorType
jvxLibHost::shutdown_deactivate_specific(jvxApiString* errloc) 
{
	return JVX_NO_ERROR;
}

jvxErrorType
jvxLibHost::shutdown_postprocess_specific(jvxApiString* errloc) 
{
	return JVX_NO_ERROR;
}

jvxErrorType
jvxLibHost::shutdown_stop_specific(jvxApiString* errloc)
{
	return JVX_NO_ERROR;
}
