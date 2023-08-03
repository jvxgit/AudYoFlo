#include "jvx.h"

#include "jvxHosts/CjvxFullHost.h"
#include <iostream>


// ===================================================================
// Some static helpers
// ===================================================================

/**
 * Check entries in list to detect existing entries
 */
static bool
isInList(std::vector<std::string> lst, std::string entry)
{
	jvxSize i;
	for (i = 0; i < lst.size(); i++)
	{
		if (lst[i] == entry)
		{
			return(true);
		}
	}
	return(false);
}

// ====================================================================
// Member functions: Allocate/deallocate object
// ====================================================================

/**
 * Constructor: Only those commands required on this class derivation level.
 */
CjvxFullHost::CjvxFullHost(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxComConHostTpl< CjvxInterfaceHostTplConnections<IjvxHost, CjvxComponentHostTools>>(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.tp = JVX_COMPONENT_HOST;
	_common_set.theComponentType.slotid = JVX_SIZE_UNSELECTED;
	_common_set.theComponentType.slotsubid = JVX_SIZE_UNSELECTED;

	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxHost*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);

	_common_set_host.reportOnConfig = NULL;
	_common_set_host.reportUnit = NULL;

	_common_set_sequencer.inOperation = NULL;

	myUniqueIdSelf = 1;
	hostUniqueId = myUniqueIdSelf + 1;

	_common_set.theInterfaceFactory = static_cast<IjvxInterfaceFactory*>(this);

	this->_set_unique_descriptor("main host");

	// Create these reference to drive the mixin classes corectly
	datConns = static_cast<IjvxDataConnections*>(this);
	uIdInst = static_cast<IjvxUniqueId*>(this);
	hIfRef = static_cast<IjvxHiddenInterface*>(this);
	myObjectRef = static_cast<IjvxObject*>(this);

	/*
	myModuleName = _common_set.theModuleName;
	myDescription = _common_set_min.theDescription;
	myDescriptor = _common_set.theDescriptor;
	*/
}

/**
 * Destructor: Only terminate call
 */
CjvxFullHost::~CjvxFullHost()
{
	this->terminate();
}

// ====================================================================
// Member functions: State transitions
// ====================================================================

/*
 * Initialize the current component
 */
jvxErrorType
CjvxFullHost::initialize(IjvxHiddenInterface* theOtherhost)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (theOtherhost == NULL)
	{
		theOtherhost = static_cast<IjvxHiddenInterface*>(this);
	}
	else
	{
		// Does this happen?? A host in a host would not make sense..
		assert(0);
	}

	res = _initialize(theOtherhost);
	if (res == JVX_NO_ERROR)
	{
		_common_set_host.threadId_registered = JVX_GET_CURRENT_THREAD_ID();

		reset();
	}
	return(res);
}

jvxErrorType
CjvxFullHost::select(IjvxObject* theOwner)
{
	jvxErrorType res = _select(theOwner);
	if (res == JVX_NO_ERROR)
	{
		_common_set.theComponentType.slotid = 0;
		_common_set.theComponentType.slotsubid = 0;

		// The unique id is obtained in the select function. However, this can not happen within the host since a valid host reference must be present
		// request_unique_host_id(&_common_set.register_host_id);

		this->init__properties_selected();
		this->allocate__properties_selected();
		this->register__properties_selected(static_cast<CjvxProperties*>(this));

		this->init__ext_callbacks__properties_callbacks();
		this->allocate__ext_callbacks__properties_callbacks();
		this->register__ext_callbacks__properties_callbacks(static_cast<CjvxProperties*>(this));

		CjvxHost_genpcg::register_callbacks(static_cast<CjvxProperties*>(this),
			cb_command_post_set,
			cb_command_pre_get,
			reinterpret_cast<jvxHandle*>(this), NULL);

	}
	return(res);
}

void
CjvxFullHost::add_self_reference()
{
	// Add this host to list of accessible objects
	// All other components
	jvxOneComponentModule elm;

	elm.theObj_single = static_cast<IjvxObject*>(this);
	elm.common.isExternalComponent = true;
	elm.common.id = (jvxSize)_common_set_host.otherComponents.availableOtherComponents.size();
	elm.common.linkage.dllPath = "NONE";
	elm.common.linkage.sharedObjectEntry = NULL;
	elm.common.linkage.funcInit = NULL;
	elm.common.linkage.funcTerm = NULL;
	elm.common.tp = JVX_COMPONENT_HOST;
	elm.common.allowsMultiObjects = false;
	elm.common.externalLink.description = "main host object";
	elm.common.dllProps = 0;
	elm.common.hObject = elm.theObj_single;

	_common_set_host.otherComponents.availableOtherComponents.push_back(elm);
}

void
CjvxFullHost::remove_self_reference()
{
	auto elm = _common_set_host.otherComponents.availableOtherComponents.begin();
	for (; elm != _common_set_host.otherComponents.availableOtherComponents.end(); elm++)
	{
		if (elm->theObj_single == static_cast<IjvxObject*>(this))
		{
			_common_set_host.otherComponents.availableOtherComponents.erase(elm);
			break;
		}
	}
}

/*
 * Activate host component
 */
jvxErrorType
CjvxFullHost::activate()
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;

	res = _activate_no_text_log(); // We need to wait with text log start until components have been loaded
	if (res == JVX_NO_ERROR)
	{
		_set_system_refs(_common_set_min.theHostRef, _common_set_host.reportUnit);

		CjvxHost_genpcg::allocate__ext_interfaces__properties_active();
		CjvxHost_genpcg::register__ext_interfaces__properties_active(static_cast<CjvxProperties*>(this));

		// ===================================================================================================

		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.component_path);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.configure_parts);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.do_unload_dlls);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.host_output_cout);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.host_verbose_dll);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_activate);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_dbglevel);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_expressions);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_filename);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferFile);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferRW);
		_update_property_access_type(JVX_PROPERTY_ACCESS_READ_ONLY, CjvxHost_genpcg::properties_selected.textLog_sizeTransferFile);

		if (CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_restart.value == NULL)
		{
			CjvxHost_genpcg::ext_interfaces.properties_active.trigger_restart.isValid = false;
		}
		if (CjvxHost_genpcg::ext_callbacks.properties_callbacks.cb_shutdown.value == NULL)
		{
			CjvxHost_genpcg::ext_interfaces.properties_active.trigger_shutdown.isValid = false;
		}

		load_dynamic_objects();

		// add_self_reference();
		for (i = 0; i < _common_set_host.otherComponents.availableOtherComponents.size(); i++)
		{
			if (_common_set_host.otherComponents.availableOtherComponents[i].theGlob_single)
			{
				_common_set_host.otherComponents.availableOtherComponents[i].theGlob_single->single_instance_post_bootup(static_cast<IjvxHiddenInterface*>(this));
			}
		}

		if (CjvxHost_genpcg::properties_selected.textLog_activate.value == c_true)
		{
			if (_common_set.theToolsHost)
			{
				jvxErrorType resL = _common_set.theToolsHost->reference_tool(JVX_COMPONENT_SYSTEM_TEXT_LOG,
					&jvxrtst_bkp.theTextLogger_obj, 0, NULL);
				if ((resL == JVX_NO_ERROR) && jvxrtst_bkp.theTextLogger_obj)
				{
					resL = jvxrtst_bkp.theTextLogger_obj->request_specialization(reinterpret_cast<jvxHandle**>(&jvxrtst_bkp.theTextLogger_hdl), NULL, NULL);
				}
			}

			if (jvxrtst_bkp.theTextLogger_hdl)
			{
				std::cout << "Starting text log file:" << std::endl;
				std::cout << " --> Filename = " << CjvxHost_genpcg::properties_selected.textLog_filename.value << std::endl;
				std::cout << " --> Internal file buffersize = " << CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferFile.value << std::endl;
				std::cout << " --> Internal cbuf buffersize = " << CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferRW.value << std::endl;
				std::cout << " --> File transfer size = " << CjvxHost_genpcg::properties_selected.textLog_sizeTransferFile.value << std::endl;
				std::cout << " --> Debug level = " << CjvxHost_genpcg::properties_selected.textLog_dbglevel.value << std::endl;

				jvxrtst_bkp.theTextLogger_hdl->initialize(
					static_cast<IjvxHiddenInterface*>(this),
					CjvxHost_genpcg::properties_selected.textLog_filename.value.c_str(),
					CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferFile.value,
					CjvxHost_genpcg::properties_selected.textLog_sizeTransferFile.value,
					CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferRW.value,
					CjvxHost_genpcg::properties_selected.textLog_dbglevel.value);

				std::cout << " --> Module selection expression = " << std::endl;
				for (i = 0; i < CjvxHost_genpcg::properties_selected.textLog_expressions.value.size(); i++)
				{
					std::cout << " -->-->" << CjvxHost_genpcg::properties_selected.textLog_expressions.value[i] << std::endl;
					jvxrtst_bkp.theTextLogger_hdl->addTextLogExpression(CjvxHost_genpcg::properties_selected.textLog_expressions.value[i].c_str());
				}
				jvxrtst_bkp.theTextLogger_hdl->start();
			}
		}
	}

	return(res);
}

jvxErrorType
CjvxFullHost::unselectAllComponents()
{
	jvxSize i, j, k;
	// A little bit of complicated since the list of slots is resized on every unselect!!
	for (i = 0; i < _common_set_types.registeredTechnologyTypes.size(); i++)
	{
		while (_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech.size())
		{
			for (j = 0; j < _common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech.size(); j++)
			{
				if (_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_tech)
				{
					while (_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev.size())
					{
						for (k = 0; k < _common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev.size(); k++)
						{
							if (_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].dev)
							{
								jvxComponentIdentification tp;
								tp.tp = _common_set_types.registeredTechnologyTypes[i].selector[1];
								tp.slotid = j;
								tp.slotsubid = k;
								this->connection_factory_to_be_removed(
									tp,
									_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].cfac,
									_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].mfac);


								if (_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].cfac)
								{
									_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].dev->return_hidden_interface(
										JVX_INTERFACE_CONNECTOR_FACTORY,
										reinterpret_cast<jvxHandle*>(_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].cfac));
									_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].cfac = NULL;
								}

								if (_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].mfac)
								{
									_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].dev->return_hidden_interface(
										JVX_INTERFACE_CONNECTOR_MASTER_FACTORY,
										reinterpret_cast<jvxHandle*>(_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].mfac));
									_common_set_types.registeredTechnologyTypes[i].technologyInstances.selTech[j].theHandle_shortcut_dev[k].mfac = NULL;
								}

								this->deactivate_selected_component(
									jvxComponentIdentification(_common_set_types.registeredTechnologyTypes[i].selector[1], j, k));

								// We need to pass a reference here to set the slot id /subid to unselected. 
								// If we run this termination, the slots must be reset by the caller!!
								jvxComponentIdentification unselectThis = jvxComponentIdentification(_common_set_types.registeredTechnologyTypes[i].selector[1], j, k);
								this->unselect_selected_component(unselectThis);
								break;
							}
						}
					}

					/* !! Important hint: Technologies typically do not have connector factories, therefore we do not need to report to host hook !! */
					this->deactivate_selected_component(
						jvxComponentIdentification(_common_set_types.registeredTechnologyTypes[i].selector[0], j, 0));

					// We need to pass a reference here to set the slot id /subid to unselected.
					// If we run this termination, the slots must be reset by the caller!!
					jvxComponentIdentification unselectThis = jvxComponentIdentification(_common_set_types.registeredTechnologyTypes[i].selector[0], j, 0);
					this->unselect_selected_component(unselectThis);
					break;
				}
			}
		}
	}

	for (i = 0; i < _common_set_types.registeredNodeTypes.size(); i++)
	{
		while (_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut.size())
		{
			for (j = 0; j < _common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut.size(); j++)
			{
				if (_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].obj)
				{
					// This is the second termination location. Only those functions are handled here which were not
					// completed in the first steps
					if (_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].cfac ||
						_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].mfac)
					{
						jvxComponentIdentification tp;
						tp.tp = _common_set_types.registeredNodeTypes[i].selector[0];
						tp.slotid = j;
						tp.slotsubid = 0;

						this->connection_factory_to_be_removed(
							tp,
							_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].cfac,
							_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].mfac);

						if (_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].cfac)
						{
							_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].obj->return_hidden_interface(
								JVX_INTERFACE_CONNECTOR_FACTORY,
								reinterpret_cast<jvxHandle*>(_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].cfac));
							_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].cfac = NULL;
						}
						if (_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].mfac)
						{
							_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].obj->return_hidden_interface(
								JVX_INTERFACE_CONNECTOR_MASTER_FACTORY,
								reinterpret_cast<jvxHandle*>(_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].mfac));
							_common_set_types.registeredNodeTypes[i].instances.theHandle_shortcut[j].mfac = NULL;
						}
					}
					this->deactivate_selected_component(
						jvxComponentIdentification(_common_set_types.registeredNodeTypes[i].selector[0], j, 0));


					// We need to pass a reference here to set the slot id /subid to unselected.
					// If we run this termination, the slots must be reset by the caller!!
					jvxComponentIdentification unselectThis = jvxComponentIdentification(_common_set_types.registeredNodeTypes[i].selector[0], j, 0);
					this->unselect_selected_component(unselectThis);
					break;
				}
			}
		}
	}
	return JVX_NO_ERROR;
}
/*
 * Deactivate host component
 */
jvxErrorType
CjvxFullHost::deactivate()
{
	jvxSize i;
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (_common_set_min.theState == JVX_STATE_ACTIVE)
	{
		// remove_self_reference();

		unselectAllComponents();

		jvxState stat = JVX_STATE_NONE;
		for (i = 0; i < _common_set_host.otherComponents.availableOtherComponents.size(); i++)
		{
			if (_common_set_host.otherComponents.availableOtherComponents[i].theGlob_single)
			{
				_common_set_host.otherComponents.availableOtherComponents[i].theGlob_single->single_instance_pre_shutdown();
			}
			_common_set_host.otherComponents.availableOtherComponents[i].theObj_single->state(&stat);
			assert(stat == JVX_STATE_NONE);
		}

		if (jvxrtst_bkp.theTextLogger_hdl)
		{
			jvxrtst_bkp.theTextLogger_hdl->stop();
			jvxrtst_bkp.theTextLogger_hdl->terminate();

			_common_set.theToolsHost->return_reference_tool(jvxComponentIdentification(JVX_COMPONENT_SYSTEM_TEXT_LOG, JVX_SIZE_UNSELECTED, JVX_SIZE_UNSELECTED), jvxrtst_bkp.theTextLogger_obj);
			jvxrtst_bkp.theTextLogger_obj = NULL;
			jvxrtst_bkp.theTextLogger_hdl = NULL;
		}

		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.component_path);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.configure_parts);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.do_unload_dlls);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.host_output_cout);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.host_verbose_dll);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_activate);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_dbglevel);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_expressions);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_filename);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferFile);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_sizeInternBufferRW);
		_undo_update_property_access_type(CjvxHost_genpcg::properties_selected.textLog_sizeTransferFile);

		CjvxHost_genpcg::unregister__ext_interfaces__properties_active(static_cast<CjvxProperties*>(this));
		CjvxHost_genpcg::deallocate__ext_interfaces__properties_active();

		res = _deactivate_no_text_log();
		assert(res == JVX_NO_ERROR);

		// unload all Dlls
		unload_dynamic_objects();

		_set_system_refs(nullptr, nullptr);
	}

	return(res);
}

jvxErrorType
CjvxFullHost::unselect()
{
	jvxErrorType res = JVX_NO_ERROR;
	if (_common_set_min.theState == JVX_STATE_SELECTED)
	{
		CjvxHost_genpcg::unregister_callbacks(static_cast<CjvxProperties*>(this), NULL);

		unregister__ext_callbacks__properties_callbacks(static_cast<CjvxProperties*>(this));
		deallocate__ext_callbacks__properties_callbacks();

		unregister__properties_selected(static_cast<CjvxProperties*>(this));
		deallocate__properties_selected();

		res = _unselect();
		assert(res == JVX_NO_ERROR);
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}
	return(res);
}

/*
 * Terminate host component
 */
jvxErrorType
CjvxFullHost::terminate()
{
	jvxErrorType res = JVX_NO_ERROR;
	res = _terminate();
	return(res);
}

// ====================================================================
// Member functions: Configuration
// ====================================================================



// ============================================================================================
/*
jvxErrorType
CjvxHost::set_external_widget_host(jvxHandle* theWidgetHost, jvxWidgetHostType widgetType)
{
	jvxErrorType res = JVX_NO_ERROR;
	res = JVX_ERROR_UNSUPPORTED;
	return res;
}
*/

// ====================================================================
// Member functions: IjvxHiddenInterface
// ====================================================================

jvxErrorType
CjvxFullHost::object_hidden_interface(IjvxObject** objRef)
{
	if (objRef)
	{
		*objRef = static_cast<IjvxObject*>(this);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxFullHost::request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)
{
	jvxErrorType res = JVX_NO_ERROR;

		switch (tp)
		{

		case JVX_INTERFACE_PROPERTY_ATTACH:
			if (hdl)
			{
				*hdl = reinterpret_cast<jvxHandle*>(static_cast<IjvxPropertyAttach*>(this));
			}
			return(JVX_NO_ERROR);

		case JVX_INTERFACE_SEQUENCER:
			if (hdl)
			{
				*hdl = static_cast<IjvxSequencer*>(this);
			}
			break;

		default:

			res = CjvxComConHostTpl< CjvxInterfaceHostTplConnections<IjvxHost, CjvxComponentHostTools>>::request_hidden_interface(tp, hdl);
		
	}
	return(res);
}

jvxErrorType
CjvxFullHost::return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)
{
	jvxErrorType res = JVX_NO_ERROR;
	
		switch (tp)
		{

		case JVX_INTERFACE_PROPERTY_ATTACH:
			if (hdl == reinterpret_cast<jvxHandle*>(static_cast<IjvxPropertyAttach*>(this)))
			{
				return(JVX_NO_ERROR);
			}
			return(JVX_ERROR_INVALID_ARGUMENT);


		case JVX_INTERFACE_SEQUENCER:
			if (hdl == static_cast<IjvxSequencer*>(this))
			{
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			break;		

		default:
			res = CjvxComConHostTpl< CjvxInterfaceHostTplConnections<IjvxHost, CjvxComponentHostTools>>::return_hidden_interface(tp, hdl);
		
	}
	return res;
}

void
CjvxFullHost::load_dynamic_objects()
{
	if (!config.use_only_static_objects)
	{
		this->loadAllComponents(JVX_EVALUATE_BITFIELD(CjvxHost_genpcg::properties_selected.do_unload_dlls.value.selection() & 0x1),
			CjvxHost_genpcg::properties_selected.component_path.value, (CjvxHost_genpcg::properties_selected.host_output_cout.value == c_true),
			(CjvxHost_genpcg::properties_selected.host_verbose_dll.value == c_true));
	}
}

void
CjvxFullHost::unload_dynamic_objects()
{
	if (!config.use_only_static_objects)
	{
		pre_unloadAllComponents();
		this->unloadAllComponents();
	}
}

// ===============================================================================================

// ================================================================
// Interface IjvxToolsHost
// ================================================================





